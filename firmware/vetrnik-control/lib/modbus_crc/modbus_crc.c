#include "modbus_crc.h"

/**
 * Calculate CRC-16-MODBUS.
 * @param crc CRC result. Should be initialized to MODBUS_CRC_INIT.
 * @param v new byte to by included in the CRC calculation
 */
void modbus_crc_update(modbus_crc_t * crc, uint8_t v)
{
    *crc ^= (uint16_t)v;
    for (uint_fast8_t i = 0; i < 8; i++)
    {
        if (*crc & 1)
            *crc = (*crc >> 1) ^ 0xA001;  // the second operand of ^ is the polynomial
        else
            *crc = (*crc >> 1);
    }
}


/**
 * Check the CRC on an incoming MODBUS message.
 * @param data message data. The last two bytes should be the CRC.
 * @param len sizeof(data)
 * @return CRC verification result: true if valid
 */
bool modbus_crc_check(const uint8_t * data, size_t len)
{
    modbus_crc_t crc = MODBUS_CRC_INIT;
    while (len)
    {
        modbus_crc_update(&crc, *data);
        data++;
        len--;
    }
    return crc == 0x0000;
}
