#pragma once

#include "Stream.h"
#include "settings.h"

void stream_print_MAC(Stream *response, uint8_t MAC[6]);
void stream_print_settings(Stream *response, settings_t s);
void stream_hexdump(Stream *response, const uint8_t *data, size_t len, size_t offset = 0);
