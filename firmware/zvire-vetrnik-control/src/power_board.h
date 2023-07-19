#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    bool hardware: 1;  ///< status of external enable pin
    bool software: 1;
    bool overall: 1;  ///< (hardware && software)
} enabled_t;

typedef struct {
    unsigned long retrieved_millis;
    bool valid;         ///< false before first status report is received
    uint16_t time;      ///< should increment every second
    uint8_t mode;       ///< see zvire-vetrnik-power/src/globals.h mode_t
    uint8_t duty;       ///< output duty cycle, 0-255
    uint8_t OCP_max_duty;  ///< OCP duty cycle limit
    uint16_t RPM;       ///< turbine RPM
    uint16_t voltage;   ///< V, *10
    uint16_t current;   ///< mA
    enabled_t enabled;  ///< @see enabled_t
    bool emergency;     ///< emergency stop mode, requires physical reset
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

typedef enum {
    PCOMMAND_RESET = 170,
    PCOMMAND_WDT_TEST = 171,
} power_board_command_t;

extern const char * power_board_modes[];


extern power_board_status_t power_board_status;


power_board_status_t power_board_status_read();


void power_board_set_duty(uint8_t duty);
void power_board_set_mode(power_board_mode_t mode);
void power_board_set_software_enable(bool value);
void power_board_clear_errors();
void power_board_command(power_board_command_t command);
bool power_board_REL_write(uint8_t pin, bool value);
uint8_t power_board_REL_read(uint8_t pin);
