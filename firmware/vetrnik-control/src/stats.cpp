#include "stats.h"
#include "power_board.h"
#include <Arduino.h>

stats_t stats = {0};


/**
 * Process new power board state.
 */
void stats_new_state()
{
    static unsigned long last_ms = 0;
    static bool prev_valid = false;

    unsigned long now = millis();

    if (power_board_status.valid && prev_valid)
    {
        static uint16_t power = 0;
        stats.energy_Ws += power * (unsigned long)(now - last_ms) / 1000UL / 10;

        // W * 10
        if (power_board_status.duty == 0) power = 0;
        else power = ((uint32_t)(power_board_status.voltage) * power_board_status.current)/1000;
    }

    prev_valid = power_board_status.valid;
    last_ms = millis();

    while (stats.energy_Ws >= 3600)
    {
        stats.energy_Ws -= 3600;
        stats.energy++;
    }
}
