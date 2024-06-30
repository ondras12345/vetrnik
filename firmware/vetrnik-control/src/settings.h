#pragma once
#include <stdint.h>

#define PIN_FLASH_SS    PB1
#define PIN_BUZZER      PB12
#define PIN_BUTTON1     PB13
#define PIN_BUTTON2     PB14
#define PIN_SHORT       PB15 // active low
#define PIN_LED         PA15
#define PIN_ONEWIRE     PB5
#define PIN_REL1        PB8
#define PIN_REL2        PB9
#define PIN_NTC1        PA0  // ADC
#define PIN_NTC2        PA1  // ADC
#define PIN_ETH_RST     PB10 // active low
#define PIN_SHORT_SENSE PC14  // low = shorted

#define BUTTON_DEBOUNCE 50  // ms
#define BUTTON_LONG 500  // ms

#define LISP_INIT_FILENAME "init.lisp"

#define MQTTport 1883
#define MQTTclientID "vetrnik"  // TODO make this a conf option
#define MQTTReconnectRate 20*1000UL  // in ms
#define MQTTWaitBeforeReconnect 5*1000UL  // in ms
#define MQTTWaitBeforeEthernetReset 15*60000UL  // in ms
#define MQTTtopic_prefix                    "vetrnik/"
#define MQTTtopic_tele                      MQTTtopic_prefix "tele/"
#define MQTTtopic_cmnd                      MQTTtopic_prefix "cmnd/"
#define MQTTtopic_stat                      MQTTtopic_prefix "stat/"
#define MQTTtopic_availability              MQTTtopic_tele "availability"
#define MQTTtopic_cmnd_raw                  MQTTtopic_cmnd "raw/"
#define MQTTtopic_cmnd_power_board          MQTTtopic_cmnd "power_board/"
#define MQTTtopic_cmnd_lisp                 MQTTtopic_cmnd "lisp"
#define MQTTtopic_cmnd_control              MQTTtopic_cmnd "control/"
#define MQTTtopic_cmnd_pump                 MQTTtopic_cmnd "pump"
#define MQTTtopic_cmnd_display_backlight    MQTTtopic_cmnd "display/backlight"
#define MQTTtopic_cmnd_cli                  MQTTtopic_cmnd "cli"
#define MQTTtopic_tele_raw_RX               MQTTtopic_tele "raw/RX/"
#define MQTTtopic_tele_raw_errors           MQTTtopic_tele "raw/errors"
#define MQTTtopic_tele_power_board          MQTTtopic_tele "power_board/"
#define MQTTtopic_tele_control              MQTTtopic_tele "control/"
#define MQTTtopic_tele_stats                MQTTtopic_tele "stats/"
#define MQTTtopic_tele_temperature          MQTTtopic_tele "temperature/"
#define MQTTtopic_tele_pump                 MQTTtopic_tele "pump"
#define MQTTtopic_tele_display_backlight    MQTTtopic_tele "display/backlight"

#define WATCHDOG_TIME 16000UL  // WDT is only used if this is defined
#define SHELL_TELNET

#define SENSOR_DS18B20_COUNT 10
/// How many retries --> the temperature is read SENSOR_DS18B20_RETRIES+1 times.
#define SENSOR_DS18B20_RETRIES 2

#define SETTINGS_FILENAME "settings"

// X(const_name, error_message)
#define SETTINGS_ERRORS(X) \
    X(OK, "OK") \
    X(MISSING_OPTION, "Missing option") \
    X(INVALID_OPTION, "Invalid conf option") \
    X(MISSING_VALUE, "Missing value") \
    X(INVALID_IP, "Invalid format, expected d.d.d.d") \
    X(INVALID_MAC, "Invalid format, expected xx:xx:xx:xx:xx:xx") \
    X(STR_TOO_LONG, "String too long") \
    X(INVALID_UINT8, "Value not in range 0-255") \
    X(INVALID_DS18B20, "Bad format") \
    X(INVALID_DS18B20_ID, "Bad id")

#define X_ENUM(const_name, error_message) SETTINGS_E_##const_name,
typedef enum {
    SETTINGS_ERRORS(X_ENUM)
} settings_parse_error_t;
#undef X_ENUM


typedef struct {
    uint8_t address[8];
    char name[16];
} settings_DS18B20_t;


#define COMMA ,
// X(printer_name, data_type, array_len?, name, default_value)
// array_len: [array_length] || : number_of_bits
// cppcheck-suppress preprocessorErrorDirective
#define CONF_ITEMS(X) \
    X(MAC,  uint8_t,  [6],    ETH_MAC, { 0xDE COMMA 0xAD COMMA 0xBE COMMA 0xEF COMMA 0xFE COMMA 0xED }) \
    X(IP,   uint8_t,  [4],    ETH_IP, {0}) /* IPv4 */ \
    X(IP,   uint8_t,  [4],    MQTTserver, {0}) /* IPv4 */ \
    X(str,  char,     [16],   MQTTuser, "") \
    X(str,  char,     [32],   MQTTpassword, "") \
    X(int,  uint8_t,  ,       DS18B20_sampling, 10) /* seconds, no sampling if 0 */ \
    X(DS18B20, settings_DS18B20_t, [SENSOR_DS18B20_COUNT], DS18B20s, {0}) /* default: all zeros */ \
    X(str,  char,     [16],   OTAname, "vetrnik") /* probably for mDNS, untested */ \
    X(str,  char,     [32],   OTApassword, "pass") \
    X(bool, bool,     :1,     shell_telnet, true) /* important: default true to prevent lockout */ \
    X(bool, bool,     :1,     report_raw, false) /* MQTT report of raw datapoints */ \
    X(int,  uint8_t,  ,       contactor_debounce_min, 15) \
    X(int,  uint8_t,  ,       DHCP_timeout_s, 14) /* default timeout in Ethernet.h is 60 s ; needs to be lower than WATCHDOG_TIME */ \
    X(int,  uint8_t,  ,       MQTT_timeout_s, 4) /* default timeout is 15 s */


// #undef COMMA - cannot undef, needed during expansion

#define X_STRUCT(printer, type, arr, name, default) type name arr;

typedef struct {
    CONF_ITEMS(X_STRUCT)
} settings_t;

#undef X_STRUCT

extern settings_t settings;
extern const settings_t settings_default;


settings_parse_error_t settings_parse(char * line, settings_t * s);
const char * settings_parse_error_message(settings_parse_error_t);
void settings_init();
void settings_write(const settings_t & s);
void settings_print(const settings_t * s, void(print_line)(void*, char *), void* ctx, bool conf_prefix);
