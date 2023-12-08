#pragma once
#include <stdbool.h>
#include <stdint.h>

#define ONEWIRE_ADDRESS_LENGTH 8

bool parse_onewire_address(uint8_t (&result)[ONEWIRE_ADDRESS_LENGTH], const char * str);

bool parse_MAC(uint8_t (&result)[6], const char * str);
