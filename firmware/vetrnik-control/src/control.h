#pragma once
#include <wt_hal.h>


void control_init();

void control_new_state();
void control_loop();

void control_set_strategy(control_strategy_t s);
bool control_set_strategy(const char * str);
control_strategy_t control_get_strategy();

void control_contactor_set();
unsigned long control_contactor_get();
