#include "settings.h"
#include <EEPROM.h>
#include "debug.h"

/*
 * https://github.com/stm32duino/wiki/wiki/API#EEPROM-Emulation
 */

settings_t settings;

settings_t settings_default = {
    { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED },
    { 0, 0, 0, 0},
    { 0, 0, 0, 0},
    "",
    "",
};

#define EEPROM_magic_length 4
static const uint8_t EEPROM_magic[EEPROM_magic_length] = { 0x00, 0x55, 0xAA, 0xFF };

static void magic_write(uint8_t start = 0)
{
    for (uint8_t i = 0; i < EEPROM_magic_length; i++)
        eeprom_buffered_write_byte(i + start, EEPROM_magic[i]);
}


static bool magic_verify(uint8_t start = 0)
{
    for (uint8_t i = 0; i < EEPROM_magic_length; i++)
    {
        if (eeprom_buffered_read_byte(i+start) != EEPROM_magic[i])
            return false;
    }
    return true;
}


static void settings_reset()
{
    INFO.println("Settings reset");
    settings = settings_default;
    magic_write(0);
    magic_write(EEPROM_magic_length + sizeof(settings_t));
    // eeprom_buffer_flush() is called in settings_write()
    settings_write(settings);
}


template<typename T> T eeprom_buffered_get(int idx, T &t)
{
    uint8_t *ptr = (uint8_t *) &t;
    for (int count = sizeof(T); count; count--, idx++)
    {
        *ptr++ = eeprom_buffered_read_byte(idx);
    }
    return t;
}

template<typename T> void eeprom_buffered_put(int idx, const T &t)
{
    const uint8_t *ptr = (const uint8_t *) &t;
    for (int count = sizeof(T); count; count--, idx++)
    {
        eeprom_buffered_write_byte(idx, *ptr++);
    }
}


void settings_init()
{
    eeprom_buffer_fill();
    if (!magic_verify() ||
        !magic_verify(EEPROM_magic_length + sizeof(settings_t)))
    {
        settings_reset();
    }
    DEBUG.println("EEPROM reading settings");
    // EEPROM.get(EEPROM_magic_length, settings);
    eeprom_buffered_get(EEPROM_magic_length, settings);
}


void settings_write(const settings_t & s)
{
    DEBUG.println("Settings write");
    // EEPROM emulation does 1 flash page write per written byte
    // EEPROM.put(EEPROM_magic_length, s);
    eeprom_buffered_put(EEPROM_magic_length, s);
    eeprom_buffer_flush();
}
