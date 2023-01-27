#include "Stream_utils.h"
#include <IPAddress.h>

void stream_print_MAC(Stream *response, uint8_t MAC[6])
{
    response->printf(
        "%02x:%02x:%02x:%02x:%02x:%02x",
        MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]
    );
}


void stream_print_settings(Stream *response, settings_t s)
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
void stream_hexdump(Stream *response, const uint8_t * data, size_t len, size_t offset)
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
