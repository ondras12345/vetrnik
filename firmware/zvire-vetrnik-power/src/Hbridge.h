#pragma once

#include <stdint.h>
#include <stdbool.h>

#define Hbridge_duty_max 247
#define Hbridge_frequency 50  // Hz ; do NOT modify without changing prescaler

void Hbridge_init();
void Hbridge_set_duty(uint8_t duty_cycle);
void Hbridge_set_enabled(bool value);
