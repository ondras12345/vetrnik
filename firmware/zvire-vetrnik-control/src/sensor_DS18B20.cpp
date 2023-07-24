#include "sensor_DS18B20.h"
#include "onewire.h"
#include "settings.h"
#include <Arduino.h>
#include <DallasTemperature.h>

//! Temperatures in 'C *100; 0 if invalid
uint16_t sensor_DS18B20_readings[SENSOR_DS18B20_COUNT] = { 0 };


static DallasTemperature sensors(&oneWire);


void sensor_DS18B20_init()
{
    sensors.begin();

    sensors.setResolution(12);
    sensors.setWaitForConversion(false);
}


/// Returns true if the sensor is enabled in settings, false if disabled or invalid.
bool sensor_DS18B20_enabled(uint_fast8_t id)
{
    if (id >= SENSOR_DS18B20_COUNT) return false;
    return settings.DS18B20s[id].name[0] != '\0';
}


/// Return fixed point number with two decimal places (temperature*100).
static uint16_t sensor_DS18B20_read(const uint8_t* device_address)
{
    unsigned int result = 0;
    int32_t temperature_raw = sensors.getTemp(device_address);
    if (temperature_raw == DEVICE_DISCONNECTED_RAW || temperature_raw < 0)
    {
        return 0;
    }
    result = (unsigned int)((temperature_raw*100) >> 7);
    // cppcheck-suppress knownConditionTrueFalse
    // Not sure if this is a false positive, keeping condition just to be safe.
    if (result >= 120*100) return 0;
    return result;
}


typedef enum {
    idle = 0,
    request,
    waiting_for_read,
    read,
    done,
} state_t;


void sensor_DS18B20_loop()
{
    if (settings.DS18B20_sampling == 0) return;

    const unsigned long now = millis();
    const unsigned long sampling_interval = settings.DS18B20_sampling * 1000UL;
    static state_t state = idle;
    static uint_fast8_t sensor_id;
    static unsigned long idle_prev_millis = now - sampling_interval;
    static unsigned long read_prev_millis = 0;
    static uint16_t readings_buff[SENSOR_DS18B20_COUNT];
    static bool error;
    static uint_fast8_t retry;

    switch (state) {
        case idle:
            if (now - idle_prev_millis >= sampling_interval)
            {
                idle_prev_millis = now;
                memset(readings_buff, 0, sizeof readings_buff);
                state = request;
                retry = 0;
            }
            break;

        case request:
            read_prev_millis = now;
            sensors.requestTemperatures();
            state = waiting_for_read;
            error = false;
            break;

        case waiting_for_read:
            // 750UL should suffice for 12-bit, 800UL just to be on the safe side
            if (now - read_prev_millis >= 800UL)
            {
                state = read;
                sensor_id = 0;
            }
            break;

        case read:
            if (sensor_id >= SENSOR_DS18B20_COUNT)
            {
                if (error && retry < SENSOR_DS18B20_RETRIES)
                {
                    retry++;
                    state = request;
                }
                else state = done;
                break;
            }

            if (sensor_DS18B20_enabled(sensor_id) && readings_buff[sensor_id] == 0)
            {
                uint16_t reading = sensor_DS18B20_read(
                    settings.DS18B20s[sensor_id].address
                );
                readings_buff[sensor_id] = reading;
                error |= (reading == 0);
            }

            sensor_id++;
            break;

        case done:
            memcpy(sensor_DS18B20_readings, readings_buff,
                   sizeof sensor_DS18B20_readings
                   );
            state = idle;
            break;

        default:
            // This should never happen
            state = idle;
            break;
    }
}
