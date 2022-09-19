#pragma once

#include <stdint.h>
#include <stdbool.h>

#define Hbridge_duty_max 247

void Hbridge_init();
void Hbridge_set_duty(uint8_t duty_cycle);
void Hbridge_set_enabled(bool value);
