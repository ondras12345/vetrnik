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
    uint8_t fan;        ///< fan output duty cycle, 0-255
    uint8_t error_count;
    // setting index and value ignored
} power_board_status_t;


extern power_board_status_t power_board_status;


power_board_status_t power_board_status_read();
