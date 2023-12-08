#pragma once
#include "settings.h"
#include <stdint.h>

void sensor_DS18B20_init();

void sensor_DS18B20_loop();

bool sensor_DS18B20_enabled(uint_fast8_t id);

extern uint16_t sensor_DS18B20_readings[SENSOR_DS18B20_COUNT];
