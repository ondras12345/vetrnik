#pragma once

#include <stdint.h>

typedef enum {
    /// Generator output shorted by three-phase contactor.
    /// Fault condition or fresh start.
    shorted = 0,

    /// Trying to stop the turbine without directly shorting it.
    /// Used when target water temperature is reached.
    stopping = 1,

    /// Constant duty cycle.
    const_duty = 2,

    /// Start from shorted state to const_duty.
    start = 3,
} mode_t;

extern bool enabled;
extern bool emergency;
extern bool OVP_stop;
extern mode_t OVP_mode;
extern uint8_t duty;
extern uint16_t RPM;
extern uint16_t voltage;  ///< V, *10
extern uint16_t current;  ///< mA
extern uint16_t temperature_heatsink;  ///< 'C, *10
extern uint16_t temperature_rectifier;  ///< 'C, *10
extern uint8_t fan;

extern mode_t mode;

bool set_mode(mode_t new_mode);

void set_duty(uint8_t value);

void emergency_stop();
