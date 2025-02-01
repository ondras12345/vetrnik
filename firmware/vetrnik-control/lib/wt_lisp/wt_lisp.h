/**
 * \file wt_lisp.h
 * \brief Implementation of wind turbine LISP functions using wt_hal.
 * See lisp/spec.yaml and lisp/spec.md for documentation of the implemented lisp functions.
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <fe.h>
#include <wt_hal.h>


void wt_lisp_init(fe_Context *ctx, wt_hal_t wt);


#ifdef __cplusplus
}
#endif
