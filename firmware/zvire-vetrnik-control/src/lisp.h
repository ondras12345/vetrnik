#pragma once
#include <Stream.h>
#include <stdbool.h>

void lisp_init();

void lisp_run_blind(char * code, size_t length);
void lisp_run_blind(char * code);
bool lisp_process(char * code, size_t length, Stream * response);
bool lisp_process(char * code, Stream * response);
