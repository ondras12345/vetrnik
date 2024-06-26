#include "settings.h"
#include "debug.h"
#include "log.h"
#include <parsers.h>
#include <Ethernet.h>  // for IPAddress
#include <SerialFlash.h>

settings_t settings;

#define X_DEFAULT(printer, type, arr, name, default) default,
settings_t settings_default = {
    CONF_ITEMS(X_DEFAULT)
};
#undef X_DEFAULT


/// Get error message corresponding to an error
const char * settings_parse_error_message(settings_parse_error_t e)
{
#define X_CASE(const_name, error_message) case SETTINGS_E_##const_name: return error_message;
    switch (e)
    {
        SETTINGS_ERRORS(X_CASE)
    }
    return "UNKNOWN";
#undef X_CASE
}


/*
 * Settings file format:
 * - text file, same syntax as conf command but w/o "conf " prefix
 * - 0x00 is skipped, 0xFF means EOF
 * - one option per line, maximum 80 characters per line
 */

/**
 * Parse one line of settings file.
 *
 * @param line line read from settings file, w/o \n or \r. Modified in place by
 *             strsep.
 * @param s settings structure to write the result to
 * @return error
 */
settings_parse_error_t settings_parse(char * line, settings_t * s)
{

    const char * setting_name = strsep(&line, " ");
    char * setting_value = line;
    if (setting_name == nullptr)
    {
        return SETTINGS_E_MISSING_OPTION;
    }
    else if (setting_value == nullptr)
    {
        return SETTINGS_E_MISSING_VALUE;
    }

#define scanconf_IP(name, type) \
    else if (strcmp(setting_name, #name) == 0) \
    { \
        IPAddress addr; \
        if (addr.fromString(setting_value)) \
        { \
            for (uint8_t i = 0; i < sizeof s->name; i++) \
                s->name[i] = addr[i]; \
        } \
        else return SETTINGS_E_INVALID_IP; \
    }

#define scanconf_str(name, type) \
    else if (strcmp(setting_name, #name) == 0) \
    { \
        if (strlen(setting_value) < sizeof s->name) \
        { \
            /* fill the rest of the memory with zeros */ \
            strncpy(s->name, setting_value, sizeof s->name); \
        } \
        else return SETTINGS_E_STR_TOO_LONG; \
    }

#define scanconf_int(name, type) scanconf_##type(name)

#define scanconf_uint8_t(name) \
    else if (strcmp(setting_name, #name) == 0) \
    { \
        unsigned int tmp; \
        sscanf(setting_value, "%u", &tmp); \
        if (tmp > 255) return SETTINGS_E_INVALID_UINT8; \
        s->name = (uint8_t)tmp; \
    }

#define scanconf_bool(name, type) \
    else if (strcmp(setting_name, #name) == 0) \
    { \
        s->name = (setting_value[0] == '1'); \
    }

#define scanconf_MAC(name, type) \
    else if (strcmp(setting_name, #name) == 0) \
    { \
        if (!parse_MAC(s->name, setting_value)) \
            return SETTINGS_E_INVALID_MAC; \
    }

#define scanconf_DS18B20(name_unused, type_unused) \
    else if (strcmp(setting_name, "DS18B20") == 0) \
    { \
        const char * const id = strsep(&setting_value, " "); \
        const char * const address = strsep(&setting_value, " "); \
        const char * const name = strsep(&setting_value, " "); \
        unsigned int sensor_id = 0; \
        if (setting_value[0] != '\0') return SETTINGS_E_INVALID_DS18B20; \
        sscanf(id, "%u", &sensor_id); \
        if (sensor_id >= SENSOR_DS18B20_COUNT) return SETTINGS_E_INVALID_DS18B20_ID; \
        strncpy(s->DS18B20s[sensor_id].name, name, sizeof s->DS18B20s[0].name); \
        /* strncpy does not guarantee NULL termination */ \
        s->DS18B20s[sensor_id].name[sizeof(s->DS18B20s[0].name) - 1] = '\0'; \
        parse_onewire_address(s->DS18B20s[sensor_id].address, address); \
    }

#define X_SCAN(scanner, type, arr, name, default) \
    scanconf_##scanner(name, type)
    CONF_ITEMS(X_SCAN)
#undef X_SCAN

    else
    {
        return SETTINGS_E_INVALID_OPTION;
    }

    return SETTINGS_E_OK;
}


void settings_print(
    const settings_t * s,
    void(print_line)(void*, const char *), void* ctx,
    bool conf_prefix
)
{
#define LINE_LEN 80
    char buf[LINE_LEN + sizeof "conf "];
    char * wptr;

#define PREP_WPTR \
    wptr = buf; \
    if (conf_prefix) { \
        strcpy(buf, "conf "); \
        wptr = buf + sizeof "conf "; \
    }

#define PRINT_str(name, v) \
    PREP_WPTR \
    snprintf(wptr, LINE_LEN+1, "%s %s", #name, v); \
    print_line(ctx, buf);

#define PRINT_bool(name, v) \
    PREP_WPTR \
    snprintf(wptr, LINE_LEN+1, "%s %c", #name, v ? '1' : '0'); \
    print_line(ctx, buf);

#define PRINT_int(name, v) \
    PREP_WPTR \
    snprintf(wptr, LINE_LEN+1, "%s %d", #name, v); \
    print_line(ctx, buf);

#define PRINT_MAC(name, v) \
    PREP_WPTR \
    snprintf(wptr, LINE_LEN+1, "%s %02x:%02x:%02x:%02x:%02x:%02x", \
             #name, v[0], v[1], v[2], v[3], v[4], v[5]); \
    print_line(ctx, buf);

#define PRINT_IP(name, v) \
    PREP_WPTR \
    snprintf(wptr, LINE_LEN+1, "%s %u.%u.%u.%u", #name, v[0], v[1], v[2], v[3]); \
    print_line(ctx, buf);

#define PRINT_DS18B20(name_unused, v) \
    for (uint_fast8_t i = 0; i < SENSOR_DS18B20_COUNT; i++) \
    { \
        PREP_WPTR \
        const uint8_t * addr = v[i].address; \
        snprintf(wptr, LINE_LEN+1, \
                 "DS18B20 %u %02X%02X%02X%02X%02X%02X%02X%02X %s", \
                 i, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], addr[6], addr[7], \
                 v[i].name); \
        print_line(ctx, buf); \
    }

#define X_PRINT(printer, type, arr, name, default) \
    PRINT_##printer(name, s->name)

    CONF_ITEMS(X_PRINT)
#undef X_PRINT
#undef PREP_WPTR
}


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
