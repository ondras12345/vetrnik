/**
 * \file lcd_lisp.h
 * \brief Implementation of lcd LISP functions using lcd_hal.
 * See lisp/spec.yaml and lisp/spec.md for documentation of the implemented lisp functions.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <fe.h>
#include <lcd_hal.h>


void lcd_lisp_init(fe_Context *ctx, lcd_hal_t lcd);


#ifdef __cplusplus
}
#endif
