#include "parsers.h"
#include <string.h>
#include <stdio.h>  // sscanf


/**
 * Parse a onewirebus address from string, writing the result to an array.
 * Example address: "2867E376E0013C73"
 * @return always true, TODO detect errors
 */
bool parse_onewire_address(uint8_t (&result)[ONEWIRE_ADDRESS_LENGTH], const char * str)
{
    for (uint_fast8_t i = 0; i < sizeof result; i++)
    {
        char digit[3];
        strncpy(digit, str, 2);
        digit[2] = '\0';
        int n = 0;
        unsigned int value = 0;
        sscanf(digit, "%x%n", &value, &n);
        result[i] = (uint8_t)value;
        str += n;
    }

    return true;
}


/**
 * Parse MAC address.
 * Example input: "de:ad:be:ef:fe:ed"
 * @return true on success, false on error
 */
bool parse_MAC(uint8_t (&result)[6], const char * str)
{
    unsigned int MAC[6];
    int scanned = sscanf(
        // newlib-nano does not support %hhx
        str, "%x:%x:%x:%x:%x:%x",
        &MAC[0], &MAC[1], &MAC[2], &MAC[3], &MAC[4], &MAC[5]
    );
    if (scanned != sizeof MAC / sizeof MAC[0])
        return false;

    for (uint_fast8_t i = 0; i < sizeof MAC / sizeof MAC[0]; i++)
        result[i] = (uint8_t)MAC[i];
    return true;
}
