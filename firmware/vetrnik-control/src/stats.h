#pragma once
#include <stdint.h>

typedef struct {
    /// Energy since last reset in Wh
    uint32_t energy;
    /// Energy in Ws
    uint16_t energy_Ws;
} stats_t;


extern stats_t stats;


void stats_new_state();
