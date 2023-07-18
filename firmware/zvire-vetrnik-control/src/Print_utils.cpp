#include "Print_utils.h"
#include <IPAddress.h>

void stream_print_MAC(Print *response, uint8_t MAC[6])
{
    response->printf(
        "%02x:%02x:%02x:%02x:%02x:%02x",
        MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]
    );
}


/**
 * Print settings_t in a format that allows it to be imported by pasting the
 * output to the cli (conf command).
 */
void stream_print_settings(Print *response, settings_t s)
{
    response->print("conf ETH_MAC ");
    stream_print_MAC(response, s.ETH_MAC);
    response->println();

    response->print("conf ETH_IP ");
    response->println(IPAddress(s.ETH_IP));

    response->print("conf MQTTserver ");
    response->println(IPAddress(s.MQTTserver));

    response->print("conf MQTTuser ");
    response->println(s.MQTTuser);

    response->print("conf MQTTpassword ");
    response->println(s.MQTTpassword);

    response->print("conf DS18B20_sampling ");
    response->println(s.DS18B20_sampling);

    for (uint_fast8_t i = 0; i < SENSOR_DS18B20_COUNT; i++)
    {
        response->print("conf DS18B20 ");
        response->print(i);
        response->print(" ");
        stream_print_onewire_address(response, s.DS18B20s[i].address);
        response->print(" ");
        response->println(s.DS18B20s[i].name);
    }

    response->print("conf OTAname ");
    response->println(s.OTAname);

    response->print("conf OTApassword ");
    response->println(s.OTApassword);

    response->print("conf shell_telnet ");
    response->println(s.shell_telnet ? '1' : '0');

    response->print("conf report_raw ");
    response->println(s.report_raw ? '1' : '0');
}


/**
 * Dump data in HEX.
 *
 * @param data data to dump
 * @param len length of data
 * @param offset Offset to be added to printed addresses.
 *               This offset is NOT added to actual data addresses.
 */
void stream_hexdump(Print *response, const uint8_t * data, size_t len, size_t offset)
{
    if (len == 0) return;
    response->print("        ");
    for (size_t i = offset; i < offset+16; i++)
    {
        response->printf(" %x ", i % 16);
    }

    for (size_t i = 0; i < len; i++)
    {
        if (i % 16 == 0)
        {
            response->printf("\r\n%08x", i+offset);
        }
        response->printf(" %02x", data[i]);
        if (i % 16 == 15 || i == len-1)
        {
            for (uint8_t k = 15 - (i % 16); k > 0; k--) response->print("   ");
            response->print("  ");
            for (size_t j = i & ~0x0F; j <= i; j++)
            {
                uint8_t c = data[j];
                if (isprint(c)) response->write(c);
                else response->write('.');
            }
        }
    }
}


void stream_print_onewire_address(Print *response, uint8_t address[8])
{
    for (uint_fast8_t i = 0; i < 8; i++)
    {
        if (address[i] < 16) response->print('0');
        response->print(address[i], HEX);
    }
}
