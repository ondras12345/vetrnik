#pragma once
#include <stdint.h>

// OVP
#define voltage_protection_start 500  // 50V
#define voltage_protection_stop 1000  // 100V
// voltage_protection_short should be significantly higher than
// voltage_protection_stop. We do not want to short the turbine while it is
// spinning.
#define voltage_protection_short 1600  // 160V

// OCP
#define OCP_L_MAX 40000U  // 40 A
#define OCP_H_MAX 42000U  // 42 A

// resistor values in voltmeter voltage divider
#define voltage_R1 100000UL
#define voltage_R2 3300

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


#define HEATSINK_TEMPERATURE_MAX 400
// see rectifier datasheet (SKBPC5004) - Io-Tc derating curve
#define FAN_TEMPERATURE_RECTIFIER_THRESHOLD 450
#define RECTIFIER_TEMPERATURE_MAX 650

#define EEPROM_RESET_ADDRESS ((uint8_t *)511)
#define EEPROM_RESET_VALUE 0x5A


// X(id, name, default_value)
#define SETTINGS_ITEMS(X) \
    /* 0 ... 50Hz ; 1 ... 400 Hz */ \
    X(0, kHBridgeFrequency, 1) \
    X(1, kRPMConversion, 10) \
    /* ACS712-20A: 5000 mV / 1024 / 100 mV/A * 1000 mA/A = 48 */ \
    /* ACS770LCB-50U: 5000 mV / 1024 / 80 mV/A * 1000 mA/A = 61 */ \
    /* ACS770LCB-50U with opamp: 5000 mV / 1024 / (80 mV/A * 1.5) * 1000 mA/A = 41 */ \
    X(2, KCurrentConversion, 41) \
    /* 500 mV .. 102; seems to want 104 */ \
    /* for opamp board with Vref set to 440mV: 21 */ \
    /* L, H - low byte, high byte */ \
    X(3, kCurrentOffsetL, 21) \
    X(4, kCurrentOffsetH, 0) \
    /* L, H - lower limit 40 A / 0.2 A = 200; upper limit 42 A / 0.2 A = 210 */ \
    X(5, kOCPL, 200) \
    X(6, kOCPH, 210) \
    /* kRPMfilter - see RPM.cpp ; was 4 - too low, try 30 */ \
    X(7, kRPMfilter, 30) \
    X(8, kMinDuty, 4) \
    X(9, kStoppingRPM, 10) \
    /* stored /10 ms */ \
    X(10, kReportRate, 50) \
    /* fan temperatures, stored *2 'C */ \
    /* need to keep heatsink cooler than 35'C at 30W / mosfet */ \
    X(11, kFanTempFull, 64) \
    /* should be higher than normal room temperature */ \
    X(12, kFanTempOff, 60)

#define X_ENUM(id, name, default_value) name = id,
typedef enum {
    SETTINGS_ITEMS(X_ENUM)
    kSettingsEnd,
} setting_index_t;
#undef X_ENUM


typedef struct {
    uint8_t default_value;
    uint8_t value;
} setting_t;

extern setting_t settings[kSettingsEnd];

void settings_init();
void settings_write(uint8_t index, uint8_t value);
