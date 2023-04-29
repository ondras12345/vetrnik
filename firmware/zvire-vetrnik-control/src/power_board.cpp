#include "power_board.h"
#include "power_datapoints.h"
#include <Arduino.h>

#define X_STR(name, value) #name,
/**
 * Array with strings corresponding to power_board_mode_t values.
 * Terminated by nullptr.
 *
 * Should NOT be used for printing out states from power_board_status_t
 * as the power board might have newer firmware than this, and could thus know
 * more states (out-of-bounds).
 *
 * Useful for performing reverse lookup.
 */
const char * power_board_modes[] = {
    POWER_BOARD_MODES(X_STR)
    nullptr
};
#undef X_STR

power_board_status_t power_board_status = { 0 };


/**
 * Parse status from RX_datapoints
 */
power_board_status_t power_board_status_read()
{
    power_board_status_t ret = { 0 };
    ret.valid = true;
    ret.retrieved_millis = millis();
    ret.time = RX_datapoints_get('t').value;
    ret.mode = RX_datapoints_get('m').value;
    ret.duty = RX_datapoints_get('d').value;
    ret.OCP_max_duty = RX_datapoints_get('C').value;
    ret.RPM = RX_datapoints_get('r').value;
    ret.voltage = RX_datapoints_get('v').value;
    ret.current = RX_datapoints_get('i').value;
    ret.enabled = RX_datapoints_get('e').value;
    ret.emergency = RX_datapoints_get('S').value;
    ret.temperature_heatsink = RX_datapoints_get('T').value;
    ret.temperature_rectifier = RX_datapoints_get('R').value;
    ret.fan = RX_datapoints_get('f').value;
    ret.error_count = RX_datapoints_get('E').value;
    return ret;
}


/**
 * Set requested duty cycle on power board.
 */
void power_board_set_duty(uint8_t duty)
{
    TX_datapoints_set('d', duty);
}


/**
 * Set requested mode on power board.
 */
void power_board_set_mode(power_board_mode_t mode)
{
    TX_datapoints_set('m', mode);
}


/**
 * Clear all errors on power board.
 */
void power_board_clear_errors()
{
    TX_datapoints_set('E', 0);
}


/**
 * Execute command on power board.
 * @see power_board_command_t
 */
void power_board_command(power_board_command_t command)
{
    TX_datapoints_set('C', command);
}


static uint8_t REL_status = 0;

/**
 * Control power board REL outputs.
 * @param pin REL output (1...4)
 * @param value true (connected to GND) or false (open collector)
 * @return true on success, else false
 */
bool power_board_REL_write(uint8_t pin, bool value)
{
    if (pin == 0 || pin > 4) return false;
    if (value) REL_status |= (1<<(pin-1));
    else REL_status &= ~(1<<(pin-1));
    TX_datapoints_set('R', REL_status);
    return true;
}


/**
 * Read status of power board REL output.
 * @param pin REL output (1...4)
 * @return 0 if off (open collector), 1 if on, -1 on error
 * Warning: this does not actually read the state from the power board, instead
 * it returns an internally kept state, which is sent to the power board every
 * 1000 ms (@see power_datapoints.cpp TX_datapoints).
 */
uint8_t power_board_REL_read(uint8_t pin)
{
    if (pin == 0 || pin > 4) return -1;
    return (REL_status & (1<<(pin-1))) ? 1 : 0;
}
