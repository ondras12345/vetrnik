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
#define MQTTclientID "vetrnik"
#define MQTTReconnectRate 20*1000UL  // in ms
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


typedef struct {
    uint8_t ETH_MAC[6];
    uint8_t ETH_IP[4];  // IPv4
    uint8_t MQTTserver[4];  // IPv4
    char MQTTuser[16];
    char MQTTpassword[32];
    uint8_t DS18B20_sampling;  // seconds, no sampling if 0
    settings_DS18B20_t DS18B20s[SENSOR_DS18B20_COUNT];
    char OTAname[16];  // probably for mDNS, untested
    char OTApassword[32];
    bool shell_telnet : 1;
    bool report_raw : 1; /// MQTT report of raw datapoints
    // remember to adjust stream_print_settings in Print_utils
    // and cmnd_conf in cli.cpp
    // and default in settings.cpp
} settings_t;

extern settings_t settings;


void settings_init();
void settings_write(const settings_t & s);
