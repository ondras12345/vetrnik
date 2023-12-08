#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <Print.h>

void lisp_init();
void lisp_reinit();

bool lisp_run_blind(const char * code, size_t length);
bool lisp_run_blind(const char * code);
bool lisp_run_blind_file(const char * filename, uint32_t offset = 0);
bool lisp_process(const char * code, size_t length, Print * response);
bool lisp_process(const char * code, Print * response);
void lisp_empty_func(const char * name);
