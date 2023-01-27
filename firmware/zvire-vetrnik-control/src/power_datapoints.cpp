#include "power_datapoints.h"
#include "debug.h"
#include "uart_power.h"  // Pserial

RX_datapoint_t RX_datapoints['~' - '!' + 1];


typedef struct {
    const char name;
    const uint16_t refresh_ms;
    uint8_t value;
    unsigned long last_refresh;
} TX_datapoint_t;

static TX_datapoint_t TX_datapoints[] = {
    {'d', 1000},
};


RX_datapoint_t RX_datapoints_get(char c)
{
    if (c < '!' || c > '~') return {0, 0};
    return RX_datapoints[c - '!'];
}


void RX_datapoints_set(char c, RX_datapoint_t v)
{
    if (c < '!' || c > '~') return;
    RX_datapoints[c - '!'] = v;
}


void TX_datapoints_set(char c, uint8_t value)
{
    for (uint8_t i = 0; i < sizeof(TX_datapoints)/sizeof(TX_datapoint_t); i++)
    {
        if (TX_datapoints[i].name == c)
        {
            TX_datapoints[i].value = value;
            TX_datapoints[i].last_refresh = millis();
        }
    }

    DEBUG.printf("Raw write: %c%03d\r\n", c, value);
    PSerial.printf("%c%03d", c, value);
}


void power_datapoints_loop()
{
    unsigned long now = millis();
    for (uint8_t i = 0; i < sizeof(TX_datapoints)/sizeof(TX_datapoint_t); i++)
    {
        if (now - TX_datapoints[i].last_refresh >= TX_datapoints[i].refresh_ms)
        {
            char c = TX_datapoints[i].name;
            uint8_t value = TX_datapoints[i].value;
            DEBUG.printf("Raw write refresh: %c%03d\r\n", c, value);
            PSerial.printf("%c%03d", c, value);
            TX_datapoints[i].last_refresh = now;
        }
    }
}
