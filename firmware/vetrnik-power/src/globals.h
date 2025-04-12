#pragma once

#include <stdint.h>

typedef enum {
    /// Generator output disconnected by three-phase contactor.
    /// Fault condition or fresh start.
    mode_emergency = 0,

    /// Trying to stop the turbine without using the contactor.
    /// Used when target water temperature is reached (enable pin
    /// is low).
    mode_stopping = 1,

    /// Constant duty cycle.
    mode_const_duty = 2,

    /// Command to switch from mode_emergency to mode_const_duty.
    mode_start = 3,
} mode_t;


typedef struct {
    bool hardware: 1;
    bool software: 1;
} enabled_t;

extern enabled_t enabled;
extern bool emergency;
extern bool OVP_stop;
extern uint8_t OCP_max_duty;
extern uint8_t duty;
extern uint16_t RPM;
extern uint16_t voltage;  ///< V, *10
extern uint16_t current;  ///< mA
extern uint16_t temperature_heatsink;  ///< 'C, *10
extern uint16_t temperature_rectifier;  ///< 'C, *10
extern uint8_t fan;
extern uint8_t fan_manual;

extern mode_t mode;

bool set_mode(mode_t new_mode);

void set_duty(uint8_t value);

void emergency_stop();

inline bool is_enabled()
{
    return enabled.hardware && enabled.software;
}
