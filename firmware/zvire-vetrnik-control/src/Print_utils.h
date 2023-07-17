#pragma once

#include <Print.h>
#include "settings.h"

void stream_print_MAC(Print *response, uint8_t MAC[6]);
void stream_print_settings(Print *response, settings_t s);
void stream_hexdump(Print *response, const uint8_t *data, size_t len, size_t offset = 0);
void stream_print_onewire_address(Print *response, uint8_t address[8]);
