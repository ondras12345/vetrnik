#include "Print_utils.h"
#include <IPAddress.h>

void stream_print_MAC(Print *response, uint8_t MAC[6])
{
    response->printf(
        "%02x:%02x:%02x:%02x:%02x:%02x",
        MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]
    );
}


static void print_line(void * ctx, const char * line)
{
    Print * response = (Print*)ctx;
    response->println(line);
}


/**
 * Print settings_t in a format that allows it to be imported by pasting the
 * output to the cli (conf command).
 */
void stream_print_settings(Print *response, settings_t s)
{
    settings_print(&s, &print_line, response, true);
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
