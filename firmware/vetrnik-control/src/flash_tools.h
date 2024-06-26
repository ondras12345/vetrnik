#pragma once
#include <SerialFlash.h>

uint32_t flash_fill_file(SerialFlashFile * f, uint8_t v, uint32_t l);
uint32_t flash_find_byte(SerialFlashFile * f, uint8_t v, bool last);
