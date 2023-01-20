#include "settings.h"
#include <EEPROM.h>
#include "error_templates.h"

setting_t settings[kSettingsEnd] = {
    {0},  // kHBridgeFrequency
    {10},  // kRPMConversion

    // ACS712-20A: 5000 mV / 1024 / 100 mV/A * 1000 mA/A = 48
    // ACS770LCB-50U: 50000 mV / 1024 / 80 mV/A * 1000 mA/A = 61
    {61},  // KCurrentConversion

    // 500 mV .. 102; seems to want 104
    {104},  // kCurrentOffsetL
    {0},  // kCurrentOffsetH

    // TODO infinite loop after adding new setting?
};


#define EEPROM_magic_length 4
static const uint8_t EEPROM_magic[EEPROM_magic_length] = { 0x00, 0x55, 0xAA, 0xFF };

static void magic_write(uint8_t start = 0)
{
    for (uint8_t i = 0; i < EEPROM_magic_length; i++)
        EEPROM.update(i + start, EEPROM_magic[i]);
}


static bool magic_verify(uint8_t start = 0)
{
    for (uint8_t i = 0; i < EEPROM_magic_length; i++)
    {
        if (EEPROM.read(i+start) != EEPROM_magic[i])
            return false;
    }
    return true;
}


static void settings_reset()
{
    magic_write(0);

    for (uint8_t i = 0; i < kSettingsEnd; i++)
    {
        uint8_t default_value = settings[i].default_value;
        settings[i].value = default_value;
        EEPROM.update(i + EEPROM_magic_length, default_value);
    }

    magic_write(kSettingsEnd + EEPROM_magic_length);

    errm_add(errm_create(&etemplate_settings_reset));
}


void settings_init()
{
    if (!magic_verify())
    {
        settings_reset();
        return;
    }

    for (uint8_t i = 0; i < kSettingsEnd; i++)
    {
        settings[i].value = EEPROM.read(i + EEPROM_magic_length);
    }

    if (!magic_verify(kSettingsEnd + EEPROM_magic_length))
    {
        settings_reset();
    }
}


void settings_write(uint8_t index, uint8_t value)
{
    if (index >= kSettingsEnd) return;
    EEPROM.update(index + EEPROM_magic_length, value);
    // not updating settings array, need to reset
    errm_add(errm_create(&etemplate_settings_changed, index));
}
