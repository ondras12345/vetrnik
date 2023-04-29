#pragma once

// OVP
#define voltage_protection_start 500  // 50V
#define voltage_protection_stop 1400  // 140V
#define voltage_protection_short 1600  // 160V

// OCP
#define OCP_L_MAX 40000U  // 40 A
#define OCP_H_MAX 42000U  // 42 A

// resistor values in voltmeter voltage divider
#define voltage_R1 100000UL
#define voltage_R2 3300

#define stopping_RPM 10  // TODO
// Time after which turbine RPM must reduce to stopping_RPM in stopping mode
#define stopping_time 10000UL  // ms

#define RPM_conversion 10  // how many RPM per Hz

#define NTC_heatsink_Rdiv 4700
#define NTC_heatsink_beta 3977
#define NTC_heatsink_R_nom 10000
#define NTC_rectifier_Rdiv 4700
#define NTC_rectifier_beta 3977
#define NTC_rectifier_R_nom 10000

#define DEBUG false


#define FAN_TEMPERATURE_FULL 320  // need to keep heatsink cooler than 35'C at 30W / mosfet. Should be better for H-bridge, about 50'C
#define FAN_TEMPERATURE_OFF 300  // probably should be higher than normal room temperature
#define FAN_POWER_MIN 0
#define HEATSINK_TEMPERATURE_MAX 400
// see rectifier datasheet (SKBPC5004) - Io-Tc derating curve
#define FAN_TEMPERATURE_RECTIFIER_THRESHOLD 450
#define RECTIFIER_TEMPERATURE_MAX 650

#define EEPROM_RESET_ADDRESS ((uint8_t *)511)
#define EEPROM_RESET_VALUE 0x5A

#include <stdint.h>

typedef enum {
    kHBridgeFrequency = 0,
    kRPMConversion = 1,
    KCurrentConversion = 2,
    kCurrentOffsetL = 3,
    kCurrentOffsetH = 4,
    kOCPL = 5,
    kOCPH = 6,
    kSettingsEnd,
} setting_index_t;


typedef struct {
    uint8_t default_value;
    uint8_t value;
} setting_t;

extern setting_t settings[kSettingsEnd];

void settings_init();
void settings_write(uint8_t index, uint8_t value);
