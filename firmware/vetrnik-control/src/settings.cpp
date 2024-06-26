#include "settings.h"
#include "debug.h"
#include "log.h"
#include <parsers.h>
#include "flash_tools.h"
#include <Ethernet.h>  // for IPAddress

settings_t settings;

#define X_DEFAULT(printer, type, arr, name, default) default,
const settings_t settings_default = {
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
 * - one option per line, maximum 80 printable characters per line
 * - neither 0x00 nor 0xFF can occur anywhere within valid data
 * - lines are ended with \n
 * - parsing errors are ignored
 * - this should ensure backwards / forwards compatibility when changing CONF_ITEMS
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
    void(print_line)(void*, char *), void* ctx,
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
        wptr = buf + sizeof("conf ") - 1; \
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


/***
 * Find first / last byte equal to v using binary search
 *
 * @param last toggle between returning address of first match and last match
 * @return address of first/last match. -1 if no match is found.
 */
static uint32_t find_byte(SerialFlashFile * f, const uint8_t v, const bool last)
{
    uint32_t left = 0;
    uint32_t right = f->size()-1;
    uint32_t first_match = -1;
    uint32_t last_match = -1;
    while (left <= right)
    {
        uint32_t mid = (left + right) / 2;
        f->seek(mid);
        char c;
        f->read(&c, 1);
        if (c == v)
        {
            if (mid > last_match || last_match == (uint32_t)-1) last_match = mid;
            if (mid < first_match) first_match = mid;
        }

        if ((c == v) == last)
            left = mid+1;
        else
        {
            if (mid == 0) break;  // prevent infinite loop caused by right=-1 overflow
            right = mid-1;
        }
    }
    return last ? last_match : first_match;
}


void settings_init()
{
    settings = settings_default;

    INFO->println("reading settings");
    SerialFlashFile f = SerialFlash.open(SETTINGS_FILENAME);
    if (!f)
    {
        if (!SerialFlash.createErasable("settings", 64*1024))
        {
            INFO->println("Could not create file for settings");
            log_add_event_and_println(kSettingsReset, INFO);
            return;
        }

        f = SerialFlash.open(SETTINGS_FILENAME);
        if (!f)
        {
            INFO->println("Settings error: could not open");
            log_add_event_and_println(kSettingsReset, INFO);
            return;
        }
    }

    // find start: skip all 0x00 (binary search)
    uint32_t offset = find_byte(&f, 0x00, true);
    // if no 0x00 is found, offset = -1 and data starts at beginning of file
    // else, data starts at offset+1
    offset++;

    for(;;)
    {
        char buf[81];
        f.seek(offset);

        // read, fill rest with 0xFF if at the end of file
        uint32_t i = f.read(buf, sizeof buf);
        for (; i < sizeof buf; buf[i] = 0xFF);

        if (buf[0] == 0xFF)
        {
            // end of file
            return;
        }

        for (i = 0; i < sizeof(buf) && buf[i] != '\n' && buf[i] != 0xFF; i++);
        // replace newline with '\0'
        buf[i] = '\0';
        // ignore parsing errors
        if (settings_parse(buf, &settings) != SETTINGS_E_OK)
        {
            // just a warning, continue parsing
            log_add_event_and_println(kSettingsInvalid, INFO);
        }

        offset += i+1;  // move past newline
    }

    // SerialFlash files do not need to be close()d
}


static bool out_of_space;
static void save_line(void * ctx, char * line)
{
    SerialFlashFile * f = (SerialFlashFile*)ctx;
    uint8_t i;
    for (i = 0; line[i] != '\0'; i++);
    line[i] = '\n';
    i++;  // length is i+1
    if (f->write(line, i) != i) out_of_space = true;
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

    uint8_t c;
    f.read(&c, 1);
    if (c == 0xFF)
    {
        // empty file
        f.seek(0);
    }
    else
    {
        uint32_t fill_start = find_byte(&f, 0x00, true) + 1;
        uint32_t fill_end = find_byte(&f, 0xFF, false);
        if (fill_end == (uint32_t)-1)
        {
            // no FFs left, need to erase file
            log_add_event_and_println(kSettingsErase, INFO);
            f.erase();
            f.seek(0);
        }
        else
        {
            fill_end--;

            INFO->printf("Filling 0x00 from %04x to %04x\r\n", fill_start, fill_end);
            f.seek(fill_start);
            flash_fill_file(&f, 0x00, fill_end-fill_start+1);
        }
    }

    out_of_space = false;

    settings_print(&s, save_line, &f, false);

    if (out_of_space)
    {
        log_add_event_and_println(kSettingsErase, INFO);
        f.erase();
        f.seek(0);
        settings_print(&s, save_line, &f, false);
    }
}
