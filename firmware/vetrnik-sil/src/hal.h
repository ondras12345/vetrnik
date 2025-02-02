/**
 * \file hal.h
 * \brief wt_hal and lcd_hal implementation using wt_sil_state
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "wt_sil.h"
#include <lcd_hal.h>

void sil_hal_init(wt_sil_state_t * s);

extern wt_hal_t wt_hal;
extern lcd_hal_t lcd_hal;

#ifdef __cplusplus
}
#endif
