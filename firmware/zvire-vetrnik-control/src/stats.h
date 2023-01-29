#pragma once
#include <stdint.h>

typedef struct {
    /// Energy since last reset in kWh * 100
    uint16_t energy;
} stats_t;


extern stats_t stats;


void stats_new_state();
