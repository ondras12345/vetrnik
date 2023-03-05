#pragma once
#include <stdint.h>
#include <stdbool.h>


typedef struct {
    unsigned long retrieved_millis;
    bool valid;         ///< false before first status report is received
    uint16_t time;      ///< should increment every second
    uint8_t mode;       ///< see zvire-vetrnik-power/src/globals.h mode_t
    uint8_t duty;       ///< output duty cycle, 0-255
    uint16_t RPM;       ///< turbine RPM
    uint16_t voltage;   ///< V, *10
    uint16_t current;   ///< mA
    bool enabled;       ///< status of external enable pin
    uint16_t temperature_heatsink;  ///< 'C, *10
    uint16_t temperature_rectifier;  ///< 'C, *10
    uint8_t fan;        ///< fan output duty cycle, 0-255
    uint8_t error_count;
    // setting index and value ignored
} power_board_status_t;


// mode numbers must start from 0 and be consecutive
#define POWER_BOARD_MODES(X) \
    /** Generator output shorted by three-phase contactor.
     Fault condition or fresh start. */ \
    X(shorted, 0) \
    /** Trying to stop the turbine without directly shorting it.
    Used when target water temperature is reached. */ \
    X(stopping, 1) \
    /** Constant duty cycle. */ \
    X(const_duty, 2) \
    /** Start from shorted state to const_duty. */ \
    X(start, 3)

#define X_ENUM(name, value) name = value,
typedef enum {
    POWER_BOARD_MODES(X_ENUM)
} power_board_mode_t;
#undef X_ENUM

extern const char * power_board_modes[];


extern power_board_status_t power_board_status;


power_board_status_t power_board_status_read();


void power_board_set_duty(uint8_t duty);
void power_board_set_mode(power_board_mode_t mode);
void power_board_clear_errors();
