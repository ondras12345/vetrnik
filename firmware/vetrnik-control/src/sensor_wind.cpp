#include "sensor_wind.h"
#include "settings.h"
#include "log.h"
#include <modbus_crc.h>
#include <Arduino.h>

static float vwind = 0.0/0.0;  ///< wind velocity in m/s
// read_cmd already includes the CRC
static const uint8_t read_cmd[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0A };


typedef enum {
    idle = 0,
    wait_DE_h,
    request,
    wait_DE_l,
    read,
} state_t;


void sensor_wind_init()
{
    pinMode(PIN_WIND_DE, OUTPUT);
    UART_WIND.begin(4800);
}


void sensor_wind_loop()
{
    const unsigned long sampling_interval = settings.wind_sampling * 100UL;
    if (sampling_interval == 0) return;  // sensor disabled
    const unsigned long now = millis();
    static unsigned long prev_millis = 0;
    static state_t state = idle;

    switch (state)
    {
        case idle:
            // discard unexpected received bytes
            while (UART_WIND.available()) UART_WIND.read();

            if (now - prev_millis >= sampling_interval)
            {
                prev_millis = now;
                digitalWrite(PIN_WIND_DE, HIGH);
                state = wait_DE_h;
            }
            break;

        case wait_DE_h:
            if (now - prev_millis >= SENSOR_WIND_DE_WAIT)
            {
                prev_millis = now;
                state = request;
            }
            break;

        case request:
            UART_WIND.write(read_cmd, sizeof read_cmd);
            state = read;
            break;

        case wait_DE_l:
            // UART_WIND.flush() would be an easy solution,
            // but I want to avoid a blocking call.
            if (SERIAL_TX_BUFFER_SIZE-1 - UART_WIND.availableForWrite() >= 0)
            {
                prev_millis = now;
            }

            if (now - prev_millis >= SENSOR_WIND_DE_WAIT)
            {
                digitalWrite(PIN_WIND_DE, LOW);
                prev_millis = now;
                state = read;
            }
            break;

        case read:
            if (UART_WIND.available() >= 7)
            {
                vwind = 0.0/0.0;  // NaN
                uint8_t msg[7];
                for (unsigned i = 0; i < sizeof msg; i++)
                    msg[i] = UART_WIND.read();
                if (UART_WIND.available())
                {
                    // msg too long
                    log_add_event(kWindMalformed);
                    state = idle;
                    prev_millis = now;
                    break;
                }

                const uint8_t msg_expected[] = {0x01, 0x03, 0x02};
                if (memcmp(msg, msg_expected, sizeof msg_expected))
                {
                    log_add_event(kWindMalformed);
                    state = idle;
                    break;
                }

                if (!modbus_crc_check(msg, sizeof msg))
                {
                    log_add_event(kWindCRC);
                    state = idle;
                    break;
                }

                vwind = (uint16_t)((msg[3] << 8) | msg[4]) * 0.1;
                prev_millis = now;  // success: we will wait before next reading
                state = idle;
                break;
            }

            if (now - prev_millis >= SENSOR_WIND_READ_TIMEOUT)
            {
                log_add_event(kWindTimeout);
                vwind = 0.0/0.0;  // NaN
                state = idle;
                // not updating prev_millis, request new read immediately
                break;
            }

            break;
    }
}


float sensor_wind_read()
{
    return vwind;
}
