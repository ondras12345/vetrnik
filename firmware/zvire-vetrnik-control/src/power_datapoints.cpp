#include "power_datapoints.h"
#include "debug.h"
#include "uart_power.h"  // Pserial

TX_datapoint_t power_datapoints[('z'-'a' + 1)*2];

RX_datapoint_t RX_datapoints['~' - '!' + 1];


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


void TX_datapoints_set(char c, uint8_t value) // TODO better implementation
{
    DEBUG.printf("Raw write: %c%03d\r\n", c, value);
    PSerial.printf("%c%03d", c, value);
}
