#include "settings.h"
#include "debug.h"
#include "log.h"
#include <SerialFlash.h>

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
    10,
    { 0 },  // all zeros
    "vetrnik",
    "pass",
    false,
    false
};


#define SETTINGS_FILENAME "settings"
#define MAGIC_LENGTH 4
static const uint8_t magic[MAGIC_LENGTH] = { 0x00, 0x55, 0xAA, 0xFF };


static void settings_reset()
{
    log_add_event_and_println(kSettingsReset, INFO);
    settings = settings_default;
    // Size doesn't matter, will be 64k since I need an erasable file
    if (!SerialFlash.exists("settings"))
    {
        bool retry = false;
tryagain:
        if (!SerialFlash.createErasable("settings", 1024))
        {
            INFO->println("Cannot create file for settings, erasing chip");
            if (!retry)
            {
                SerialFlash.eraseAll();
                while (!SerialFlash.ready())
                {
                    // TODO reset WDT ??
                }
                retry = true;
                goto tryagain;
            }
            INFO->println("Failed to create file for settings, even after erase");
        }
    }
    settings_write(settings);
}


void settings_init()
{
    INFO->println("reading settings");
    SerialFlashFile f = SerialFlash.open(SETTINGS_FILENAME);
    if (!f)
    {
        settings_reset();
        f = SerialFlash.open(SETTINGS_FILENAME);
        if (!f)
        {
            INFO->println("Settings error: could not open");
            return;
        }
    }

    uint8_t buf[sizeof(settings_t) + sizeof(magic)];
    f.read(buf, sizeof buf);
    if (memcmp(buf + sizeof(settings_t), magic, sizeof magic) != 0)
    {
        INFO->println("Settings: invalid magic");
        f.close();
        settings_reset();
        f = SerialFlash.open(SETTINGS_FILENAME);
        if (!f)
        {
            INFO->println("Settings error: could not open");
            return;
        }
        f.read(buf, sizeof buf);
        // magic must be right, because we just wrote it
    }
    f.close();
    memcpy(&settings, buf, sizeof settings);
}


void settings_write(const settings_t & s)
{
    log_add_event_and_println(kSettingsWrite, INFO);
    SerialFlashFile f = SerialFlash.open(SETTINGS_FILENAME);
    if (!f)
    {
        INFO->println("Could not open file during settings_write");
        return;
    }
    f.erase();  // TODO write all the way to the end of the file, erase less often
    uint8_t buf[sizeof(settings_t) + sizeof(magic)];
    memcpy(buf, &s, sizeof s);
    memcpy(buf+sizeof s, magic, sizeof magic);
    f.write(buf, sizeof buf);
    f.close();
}
