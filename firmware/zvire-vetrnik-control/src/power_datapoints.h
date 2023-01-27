#pragma once

#include <stdint.h>

typedef struct __attribute__((packed)) {
    unsigned long value : 23;
    bool changed : 1;
} RX_datapoint_t;


#if CHAR_MIN < 0
#error "plain char is signed, review RX_datapoints indexing"
#endif

RX_datapoint_t RX_datapoints_get(char c);
void RX_datapoints_set(char c, RX_datapoint_t v);

void TX_datapoints_set(char c, uint8_t value);

void power_datapoints_loop();
