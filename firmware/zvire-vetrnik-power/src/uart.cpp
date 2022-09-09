#include "uart.h"
#include <errm.h>
#include "error_templates.h"
#include "globals.h"
#include "settings.h"

/*
 * For the sake of simplicity, the design of the UART interface is as follows:
 *
 * Rx:
 * - Every message begins with a single character ([a-zA-Z]) that identifies
 *   the data point.
 * - Every data point has an 8-bit value.
 * - The 8-bit values need to be sent as 3-digit decimal numerals.
 *   Smaller numbers need to be left-padded with zeros (e.g. 001).
 *   Numbers starting with zero are NOT octal.
 * - White space between messages is optional and ignored.
 *
 * Tx:
 * - Datapoint values are reported in a similar format to Rx messages.
 *   However, the limitation on number of digits is lifted.
 * - Messages starting with '>' and terminated by '\r\n' contain debug and
 *   error messages.
 */


typedef struct {
    const char name;
    void (*process_value)(uint8_t);
    unsigned long last_updated;
    // If value isn't received at least every refresh_interval milliseconds,
    // an error is thrown.
    // Setting refresh_interval to 0 disables this feature.
    unsigned long refresh_interval;
} datapoint_t;


/*!
    @brief  Convert a string of digits to an 8-bit unsigned integer.

    Negative numbers are not supported. Overflows are not handled.
    @param str  The string.
    @return The number.
            Returns 0 if the first character of the string is not a digit.
*/
static uint8_t str_uint8(const char* str)
{
    uint8_t result = 0;
    while (isDigit(*str))
    {
        result = result * 10 + (*str - '0');
        str++;
    }
    return result;
}


static void cmnd_duty(uint8_t value)
{
    set_duty(value);
}

static void cmnd_mode(uint8_t value)
{
    switch (value)
    {
        case 0 ... 3:
            if (!set_mode((mode_t)value))
                errm_add(errm_create(&etemplate_comms_mode, value));
            break;
        default:
            errm_add(errm_create(&etemplate_comms_arg, uint8_t('m')));
            break;
    }

}

static void cmnd_errors(uint8_t value)
{
    (void) value;
    errm_clear();
}


static datapoint_t datapoints[] = {
    {'d', cmnd_duty, 0, 2100},  // duty
    {'m', cmnd_mode, 0, 5100},  // mode
    {'E', cmnd_errors, 0, 0},  // clear errors
};

#define FOR_EACH_DP for (uint8_t i = 0; i < sizeof datapoints / sizeof datapoints[0]; i++)



static void report()
{
    Serial.print("t");
    Serial.print(millis() / 1000UL);
    Serial.print(" ");

    Serial.print("m");
    Serial.print(mode);
    Serial.print(" ");

    Serial.print("d");
    Serial.print(duty);
    Serial.print(" ");

    Serial.print("r");
    Serial.print(RPM);
    Serial.print(" ");

    Serial.print("v");
    Serial.print(voltage);
    Serial.print(" ");

    Serial.print("e");
    Serial.print(enabled);
    Serial.print(" ");

    Serial.print("T");
    Serial.print(temperature_heatsink);
    Serial.print(" ");

    Serial.print("f");
    Serial.print(fan);
    Serial.print(" ");

    Serial.print("E");
    Serial.print(errm_count);

    Serial.print("\r\n");
}


void uart_init()
{
    Serial.begin(9600);  // TODO 8E1
}


void uart_loop()
{
    static bool in_msg = false;
    static datapoint_t * parsed_dp = nullptr;
    static char buff[8];  // this is NOT always null terminated!
    static uint8_t buff_index = 0;

    while (Serial.available())
    {
        char c = Serial.read();
        if (!in_msg)
        {
            FOR_EACH_DP
            {
                if (c == datapoints[i].name)
                {
                    in_msg = true;
                    parsed_dp = &datapoints[i];
                }
            }
            continue;
        }

        if (!isDigit(c))
        {
            in_msg = false;
            buff_index = 0;
            errm_add(errm_create(&etemplate_comms_malformed));
            continue;
        }

        buff[buff_index++] = c;
        // this should never happen
        if (buff_index >= sizeof buff / sizeof buff[0])
        {
            // message too long
            Serial.println(F(">comm msg too long"));
            in_msg = false;
            buff_index = 0;
            continue;
        }

        if (buff_index == 3)
        {
            buff[buff_index++] = '\0';
            if (parsed_dp->process_value) parsed_dp->process_value(str_uint8(buff));
            parsed_dp->last_updated = millis();
            in_msg = false;
            buff_index = 0;
            continue;
        }
    }


    unsigned long now = millis();
    FOR_EACH_DP
    {
        if (datapoints[i].refresh_interval && !DEBUG &&
            now - datapoints[i].last_updated >= datapoints[i].refresh_interval)
        {
            errm_add(errm_create(&etemplate_comms_timeout, uint8_t(datapoints[i].name)));
        }
    }

    static unsigned long last_error_print = 0;
    if (errm_count > 0 && now - last_error_print >= 2000UL)
    {
        last_error_print = now;
        for (uint8_t i = 0; i < errm_count; i++)
        {
            Serial.print(F(">err "));
            Serial.print(i);
            Serial.print(" ");
            Serial.print(errm_errors[i].errortemplate->text);
            Serial.print(errm_errors[i].code);
            Serial.print(" ");
            Serial.println((now - errm_errors[i].when) / 1000UL);
        }
        Serial.println();
    }

    static unsigned long last_report = 0;
    if (now - last_report >= 500UL)
    {
        last_report = now;
        report();
    }
}
