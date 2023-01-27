#pragma once
#include <stdint.h>

#define PIN_FLASH_SS PB2


#define MQTTport 1883
#define MQTTclientID "vetrnik"
#define MQTTReconnectRate 20*1000  // in ms
#define MQTTtopic_prefix                    "vetrnik/"
#define MQTTtopic_tele                      MQTTtopic_prefix "tele/"
#define MQTTtopic_cmnd                      MQTTtopic_prefix "cmnd/"
#define MQTTtopic_stat                      MQTTtopic_prefix "stat/"
#define MQTTtopic_availability              MQTTtopic_tele "availability"
#define MQTTtopic_cmnd_raw                  MQTTtopic_cmnd "raw/"
#define MQTTtopic_cmnd_power_board          MQTTtopic_cmnd "power_board/"
#define MQTTtopic_cmnd_lisp                 MQTTtopic_cmnd "lisp"
#define MQTTtopic_tele_raw_RX               MQTTtopic_tele "raw/RX/"
#define MQTTtopic_tele_raw_errors           MQTTtopic_tele "raw/errors"
#define MQTTtopic_tele_power_board          MQTTtopic_tele "power_board/"

#define WATCHDOG_TIME 16000UL  // WDT is only used if this is defined
#define SHELL_TELNET
#define LISP_REPL


typedef struct {
    uint8_t ETH_MAC[6];
    uint8_t ETH_IP[4];  // IPv4
    uint8_t MQTTserver[4];  // IPv4
    char MQTTuser[16];
    char MQTTpassword[32];
    bool shell_telnet : 1;
    bool report_raw : 1; /// MQTT report of raw datapoints
    // remember to adjust stream_print_settings in Stream_utils
    // and cmnd_conf in cli.cpp
    // and default in settings.cpp
} settings_t;

extern settings_t settings;


void settings_init();
void settings_write(const settings_t & s);
