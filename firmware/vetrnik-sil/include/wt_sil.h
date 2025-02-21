/**
 * \file wt_sil.h
 * \brief Software-in-the-loop library implementing the lisp language.
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <wt_hal.h>
#include "wt_sil_modelica.h"

#define LCD_ROWS 2
#define LCD_COLS 16

#define POWER_REL_COUNT 4

typedef struct
{
    power_board_status_t pwr_status;
    control_strategy_t ctrl_strategy;
    stats_t stats;
    bool pwr_REL[POWER_REL_COUNT];
    bool outputs[OUT_LAST_];
    char lcd[LCD_ROWS][LCD_COLS+1];
    bool lcd_backlight;
    char lcd_buf[LCD_COLS+1];
    uint8_t lcd_buf_index;
    bool ctrl_contactor_state;
    bool pump;
    float vwind;
    void * fe_ctx;
    // fe requires the buffer to be 32-bit aligned.
    uint8_t fe_buf[24*1024]
// cffi python lib does not like __attribute__
#ifndef CFFI_HEADER
        __attribute__((aligned(4)))
#endif
        ;
} wt_sil_state_t;

void wt_sil_init(wt_sil_state_t * state);
void wt_sil_close(wt_sil_state_t * state);
bool wt_sil_repl(wt_sil_state_t * state);
int wt_sil_run_file(wt_sil_state_t * state, const char * filename);
void wt_sil_run_str(wt_sil_state_t * state, const char * code);


#ifdef __cplusplus
}
#endif
