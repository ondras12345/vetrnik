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


//! Return fixed point number with two decimal places (temperature*100).
uint16_t sensor_DS18B20_read(const uint8_t* device_address)
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


void sensor_DS18B20_loop()
{
    if (settings.DS18B20_sampling == 0) return;

    static bool samples_requested = false;
    const unsigned long sampling_interval = settings.DS18B20_sampling * 1000UL;
    static unsigned long prev_millis = millis() - sampling_interval;

    if (!samples_requested &&
        millis() - prev_millis >= sampling_interval)
    {
        samples_requested = true;
        prev_millis = millis();
        sensors.requestTemperatures();
    }

    // 750UL should suffice for 12-bit, 800UL just to be on the safe side
    if (samples_requested && millis() - prev_millis >= 800UL)
    {
        // not updating prev_millis to prevent throwing off sampling_interval
        samples_requested = false;

        for (uint_fast8_t i = 0; i < SENSOR_DS18B20_COUNT; i++)
        {
            uint16_t value = 0;
            if (settings.DS18B20s[i].name[0] != '\0')
            {
                value = sensor_DS18B20_read(settings.DS18B20s[i].address);
            }

            sensor_DS18B20_readings[i] = value;
        }
    }
}
