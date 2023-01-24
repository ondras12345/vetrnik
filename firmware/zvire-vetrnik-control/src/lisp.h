#pragma once
#include <Stream.h>
#include <stdbool.h>

void lisp_init();

bool lisp_run_blind(const char * code, size_t length);
bool lisp_run_blind(const char * code);
bool lisp_process(const char * code, size_t length, Stream * response);
bool lisp_process(const char * code, Stream * response);
