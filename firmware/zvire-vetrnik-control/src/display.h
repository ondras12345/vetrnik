#pragma once
#include <stdint.h>

#define DISPLAY_COLS 16
#define DISPLAY_ROWS 2

void display_init();
void display_init_lisp();

void display_loop();

bool display_commit(uint8_t row);
void display_print(const char * str);
bool display_set_cursor(uint8_t col);
