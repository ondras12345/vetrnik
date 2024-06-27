#include "uart.h"
#include <serial.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <errm.h>
#include "error_templates.h"
#include "hardware.h"
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
    // If value isn't received at least every refresh_interval milliseconds,
    // an error is thrown.
    // Setting refresh_interval to 0 disables this feature.
    unsigned long refresh_interval;
    unsigned long last_updated;
} datapoint_t;


enum uart_command {
    COMMAND_RESET = 170,
    COMMAND_WDT_TEST = 171,
};


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

static uint8_t setting_index = 0;
static void cmnd_setting_choose(uint8_t value)
{
    if (value >= kSettingsEnd)
    {
        errm_add(errm_create(&etemplate_comms_arg, uint8_t('$')));
        return;
    }
    setting_index = value;
}

static void cmnd_setting_set(uint8_t value)
{
    settings_write(setting_index, value);
}


static void cmnd_command(uint8_t value)
{
    switch (value) {
        case COMMAND_RESET:
            serial_puts_p(PSTR(">resetting\r\n"));
            serial_flush();
            // eeprom_write_byte would be faster, but I don't have enough flash
            // space for it.
            eeprom_update_byte(EEPROM_RESET_ADDRESS, EEPROM_RESET_VALUE);
        case COMMAND_WDT_TEST:
            emergency_stop();
            // Watchdog reset
            for (;;);
            break;

        default:
            errm_add(errm_create(&etemplate_comms_arg, uint8_t('C')));
    }
}


// control REL outputs
static void cmnd_REL(uint8_t value)
{
    if (value & 0xF0) errm_add(errm_create(&etemplate_comms_arg, uint8_t('R')));
    gpio_wr(PORT, pin_REL1, value & 0x01);
    gpio_wr(PORT, pin_REL2, value & 0x02);
    gpio_wr(PORT, pin_REL3, value & 0x04);
    gpio_wr(PORT, pin_REL4, value & 0x08);
}


static void cmnd_enable(uint8_t value)
{
    enabled.software = !!value;
}


static void cmnd_fan(uint8_t value)
{
    fan_manual = value;
}


static datapoint_t datapoints[] = {
    {'d', cmnd_duty, 5100},  // duty
    {'m', cmnd_mode},  // mode
    {'E', cmnd_errors},      // clear errors
    // The following commands allow settings to be read ($index, wait for next
    // status message) and written ($index=value)
    {'$', cmnd_setting_choose},
    {'=', cmnd_setting_set},
    {'C', cmnd_command},
    {'R', cmnd_REL},
    {'e', cmnd_enable},
    {'f', cmnd_fan},
};

#define FOR_EACH_DP for (uint8_t i = 0; i < sizeof datapoints / sizeof datapoints[0]; i++)



static void report()
{
    // TX buffer size has been increased in platformio.ini
    // -DSERIAL_TX_BUFFER_SIZE=128. This isn't absolutely necessary, but it
    // should improve overall performance.

    serial_putc('t');
    // Don't print out too many digits. Filling the TX buffer is not a good
    // thing.
    serial_putuint((millis() / 1000UL) & 0xFFFF);
    serial_putc(' ');

    serial_putc('m');
    serial_putuint(mode);
    serial_putc(' ');

    serial_putc('d');
    serial_putuint(duty);
    serial_putc(' ');

    serial_putc('r');
    serial_putuint(RPM);
    serial_putc(' ');

    serial_putc('v');
    serial_putuint(voltage);
    serial_putc(' ');

    serial_putc('i');
    serial_putuint(current);
    serial_putc(' ');

    serial_putc('e');
    serial_putuint((enabled.hardware ? 1 : 0) | (enabled.software ? 2 : 0));
    serial_putc(' ');

    serial_putc('T');
    serial_putuint(temperature_heatsink);
    serial_putc(' ');

    serial_putc('R');
    serial_putuint(temperature_rectifier);
    serial_putc(' ');

    serial_putc('f');
    serial_putuint(fan);
    serial_putc(' ');

    serial_putc('E');
    serial_putuint(errm_count);
    serial_putc(' ');

    serial_putc('S');
    serial_putuint(emergency);
    serial_putc(' ');

    serial_putc('C');
    serial_putuint(OCP_max_duty);
    serial_putc(' ');

    serial_putc('$');
    serial_putuint(setting_index);
    serial_putc('=');
    serial_putuint(settings[setting_index].value);

    serial_putc('\r');
    serial_putc('\n');
}


void uart_init()
{
    serial_init9600();  // TODO 8E1
}


void uart_loop()
{
    static bool in_msg = false;
    static datapoint_t * parsed_dp = nullptr;
    static char buff[8];  // this is NOT always null terminated!
    static uint8_t buff_index = 0;

    unsigned long now = millis();

    while (serial_available())
    {
        char c = serial_read();
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
            serial_puts_p(PSTR(">comm too long\r\n"));
            in_msg = false;
            buff_index = 0;
            continue;
        }

        if (buff_index == 3)
        {
            buff[buff_index++] = '\0';
            if (parsed_dp->process_value) parsed_dp->process_value(str_uint8(buff));
            parsed_dp->last_updated = now;
            in_msg = false;
            buff_index = 0;
            continue;
        }
    }

    if (mode != shorted) FOR_EACH_DP
    {
        if (datapoints[i].refresh_interval && !DEBUG &&
            now - datapoints[i].last_updated >= datapoints[i].refresh_interval)
        {
            errm_add(errm_create(&etemplate_comms_timeout, uint8_t(datapoints[i].name)));
            // will get emergency stopped by errm callback
        }
    }

    static unsigned long last_error_print = 0;
    if (errm_count > 0 && now - last_error_print >= 2000UL)
    {
        last_error_print = now;
        for (uint8_t i = 0; i < errm_count; i++)
        {
            serial_puts_p(PSTR(">err "));
            serial_putuint(i);
            serial_putc(' ');
            serial_puts(errm_errors[i].errortemplate->text);
            serial_putuint(errm_errors[i].code);
            serial_putc(' ');
            serial_putuint((now - errm_errors[i].when) / 1000UL);
            serial_putc('\r');
            serial_putc('\n');
        }
        serial_putc('\r');
        serial_putc('\n');
    }

    static unsigned long last_report = 0;
    if (now - last_report >= settings[kReportRate].value*10UL)
    {
        last_report = now;
        report();
    }
}
