#include "settings.h"
#include <EEPROM.h>
#include "error_templates.h"

setting_t settings[kSettingsEnd] = {
    {kHBridgeFrequency, 0},
    {kRPMConversion, 10},
};


#define EEPROM_magic_length 4
static const uint8_t EEPROM_magic[EEPROM_magic_length] = { 0x00, 0x55, 0xAA, 0xFF };


void settings_reset()
{
    for (uint8_t i = 0; i < EEPROM_magic_length; i++)
        EEPROM.update(i, EEPROM_magic[i]);

    for (uint8_t i = 0; i < kSettingsEnd; i++)
    {
        uint8_t default_value = settings[i].default_value;
        settings[i].value = default_value;
        EEPROM.update(i + EEPROM_magic_length, default_value);
    }

    for (uint8_t i = 0; i < EEPROM_magic_length; i++)
        EEPROM.update(i + kSettingsEnd, EEPROM_magic[i]);
}


void settings_init()
{
    for (uint8_t i = 0; i < EEPROM_magic_length; i++)
    {
        if (EEPROM.read(i) != EEPROM_magic[i])
        {
            settings_reset();
            return;
        }
    }

    for (uint8_t i = 0; i < kSettingsEnd; i++)
    {
        settings[i].value = EEPROM.read(i + EEPROM_magic_length);
    }

    for (uint8_t i = 0; i < EEPROM_magic_length; i++)
    {
        if (EEPROM.read(i + kSettingsEnd) != EEPROM_magic[i])
        {
            settings_reset();
            return;
        }
    }
}

void settings_write(uint8_t index, uint8_t value)
{
    if (index >= kSettingsEnd) return;
    EEPROM.update(index + EEPROM_magic_length, value);
    // not updating settings array, need to reset
    errm_add(errm_create(&etemplate_settings_changed));
}
