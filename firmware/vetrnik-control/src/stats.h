#pragma once
#include <stdint.h>

typedef struct {
    /// Energy since last reset in Wh
    uint32_t energy;
    /// Sub-Wh energy in Ws*10, always less than 36000
    uint16_t energy_Ws10;
} stats_t;


extern stats_t stats;


void stats_new_state();
