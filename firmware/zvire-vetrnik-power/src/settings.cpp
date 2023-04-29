#include "settings.h"
#include "error_templates.h"
#include <avr/io.h>
#include <avr/eeprom.h>

setting_t settings[kSettingsEnd] = {
    // 0 ... 50Hz ; 1 ... 400 Hz
    {0},  // kHBridgeFrequency

    {10},  // kRPMConversion

    // ACS712-20A: 5000 mV / 1024 / 100 mV/A * 1000 mA/A = 48
    // ACS770LCB-50U: 5000 mV / 1024 / 80 mV/A * 1000 mA/A = 61
    // ACS770LCB-50U with opamp: 5000 mV / 1024 / (80 mV/A * 1.5) * 1000 mA/A = 41
    {41},  // KCurrentConversion

    // 500 mV .. 102; seems to want 104
    // for opamp board with Vref set to 440mV: 21
    // L, H - low byte, high byte
    {21},  // kCurrentOffsetL
    {0},  // kCurrentOffsetH
    // L, H - lower limit, upper limit:
    {200},  // kOCPL, 40 A / 0.2 A = 200
    {210},  // kOCPH, 42 A / 0.2 A = 210

    // TODO adding new settings causes infinite loop ??
};


#define EEPROM_update(address, data) eeprom_update_byte((uint8_t*)(address), data)
#define EEPROM_read(address) eeprom_read_byte((uint8_t*)(address))


#define EEPROM_magic_length 4
static const uint8_t EEPROM_magic[EEPROM_magic_length] = { 0x00, 0x55, 0xAA, 0xFF };

static void magic_write(uint8_t start = 0)
{
    for (uint8_t i = 0; i < EEPROM_magic_length; i++)
        EEPROM_update(i + start, EEPROM_magic[i]);
}


static bool magic_verify(uint8_t start = 0)
{
    for (uint8_t i = 0; i < EEPROM_magic_length; i++)
    {
        if (EEPROM_read(i+start) != EEPROM_magic[i])
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
        EEPROM_update(i + EEPROM_magic_length, default_value);
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
        settings[i].value = EEPROM_read(i + EEPROM_magic_length);
    }

    if (!magic_verify(kSettingsEnd + EEPROM_magic_length))
    {
        settings_reset();
    }
}


void settings_write(uint8_t index, uint8_t value)
{
    if (index >= kSettingsEnd) return;
    EEPROM_update(index + EEPROM_magic_length, value);
    // not updating settings array, need to reset
    errm_add(errm_create(&etemplate_settings_changed, index));
}
