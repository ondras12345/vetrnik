#pragma once

#define voltage_protection_start 500  // 50V
#define voltage_protection_stop 1400  // 140V
#define voltage_protection_short 1600  // 160V

#define voltage_R1 100000UL
#define voltage_R2 3300

#define stopping_RPM 10  // TODO
// Time after which turbine RPM must reduce to stopping_RPM in stopping mode
#define stopping_time 10000UL  // ms

#define RPM_conversion 10  // how many RPM per Hz

#define NTC_heatsink_Rdiv 4700
#define NTC_heatsink_beta 3977
#define NTC_heatsink_R_nom 10000

#define DEBUG true


// TODO macro naming all caps
#define fan_temperature_full 30  // need to keep heatsink cooler than 35'C at 30W / mosfet. Should be better for H-bridge, about 50'C
#define fan_temperature_off 25
#define fan_power_min 0
#define heatsink_temperature_max 40

#include <stdint.h>

typedef enum {
    kHBridgeFrequency = 0,  // 0 ... 50Hz ; 1 ... 400 Hz
    kRPMConversion = 1,
    kSettingsEnd,
} setting_index_t;


typedef struct {
    setting_index_t index;
    uint8_t default_value;
    uint8_t value;
} setting_t;

extern setting_t settings[kSettingsEnd];

void settings_init();
void settings_write(uint8_t index, uint8_t value);
