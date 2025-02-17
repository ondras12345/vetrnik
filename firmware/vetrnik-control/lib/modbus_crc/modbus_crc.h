#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define MODBUS_CRC_INIT 0xFFFF
typedef uint16_t modbus_crc_t;

void modbus_crc_update(modbus_crc_t * crc, uint8_t v);

bool modbus_crc_check(const uint8_t * data, size_t len);


#ifdef __cplusplus
}
#endif
