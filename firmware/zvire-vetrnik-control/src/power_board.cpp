#include "power_board.h"
#include "power_datapoints.h"
#include <Arduino.h>


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
    ret.fan = RX_datapoints_get('f').value;
    ret.error_count = RX_datapoints_get('E').value;
    return ret;
}
