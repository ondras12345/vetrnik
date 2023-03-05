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
    ret.RPM = RX_datapoints_get('r').value;
    ret.voltage = RX_datapoints_get('v').value;
    ret.current = RX_datapoints_get('i').value;
    ret.enabled = RX_datapoints_get('e').value;
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
