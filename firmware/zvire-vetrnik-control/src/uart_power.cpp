#include "uart_power.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <Arduino.h>
#include "mqtt.h"
#include "power_datapoints.h"

Stream * print_RX = nullptr;

char power_text_message[51] = "";  // size doesn't matter too much
bool power_text_message_complete = false;


/**
 * Parse a single character from power board's UART.
 *
 * @return true at the end of a valid word (when value is complete), else false
 */
static bool parse_char(char c, char & name, unsigned long & value)
{
    static char now_parsing = '\0';  // name of the currently parsed value

    // Used at the end of a line
    if (c == '\0')
    {
        now_parsing = '\0';
        return false;
    }

    if (now_parsing != '\0' && isDigit(c))
    {
        value = value * 10 + (c - '0');
        return false;
    }
    else
    {
        name = now_parsing;
        now_parsing = c;

        if (name >= '!' && name <= '~')
        {
            return true;
        }
        return false;
    }
}


void uart_power_init()
{
    PSerial.begin(9600);
}


/**
 * @return true if RX_datapoints has been completely updated
 */
bool uart_power_loop()
{
    static uint8_t buff_i = 0;
    static char name = '\0';
    static unsigned long value = 0;
    static bool text_message = false;
    while (PSerial.available())
    {
        char c = PSerial.read();
        if (print_RX != nullptr) print_RX->print(c);

        if (text_message)
        {
            // handle error / text message
            uint8_t i = strlen(power_text_message);
            if (i < sizeof power_text_message - 1)
            {
                power_text_message[i] = c;
                power_text_message[i+1] = '\0';
            }
        }

        if (c == '>')
        {
            text_message = true;
            power_text_message_complete = false;
            power_text_message[0] = '\0';
        }

        if ((c == '\r' || c == '\n') && text_message)
        {
            power_text_message_complete = true;
            text_message = false;
        }

        if (text_message) continue;

        if (parse_char(c, name, value))
        {
            RX_datapoints_set(name, {value, RX_datapoints_get(name).value != value});
            value = 0;
        }

        if (c == '\n')
        {
            parse_char('\0', name, value);
            value = 0;
            return true;
        }
    }
    return false;
}
