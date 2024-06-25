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


typedef struct {
    uint8_t address[8];
    char name[16];
} settings_DS18B20_t;

typedef uint8_t settings_MAC_t[6];


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
    X(bool, bool,     :1,     shell_telnet, false) \
    X(bool, bool,     :1,     report_raw, false) /* MQTT report of raw datapoints */
// #undef COMMA - cannot undef, needed during expansion

#define X_STRUCT(printer, type, arr, name, default) type name arr;

typedef struct {
    CONF_ITEMS(X_STRUCT)
} settings_t;

#undef X_STRUCT

extern settings_t settings;


void settings_init();
void settings_write(const settings_t & s);
