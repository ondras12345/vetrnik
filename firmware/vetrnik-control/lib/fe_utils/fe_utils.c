#include "fe_utils.h"
#include <math.h>

// TODO integer division "div"

/**
 * Remainder function
 *
 * rem performs the operation truncate on number and divisor and returns the
 * remainder of the truncate operation.
 *
 * https://stackoverflow.com/questions/5706398/how-to-get-the-modulus-in-lisp
 *
 * Seemed easier to implement than mod.
 */
fe_Object* cfunc_rem(fe_Context *ctx, fe_Object *arg)
{
    int x = (int)fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    int y = (int)fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    if (y == 0) fe_error(ctx, "rem divide by 0");
    return fe_number(ctx, x % y);
}


/**
 * Arduino-like map function, but less broken (uses floats).
 *
 * (map value from_min from_max to_min to_max)
 *
 * This could be easily implemented in Lisp, but a cfunc takes up less memory.
 * (= map (fn (v fl fh tl th)
 *   (+ tl
 *     (/ (* (- v fl) (- th tl)) (- fh fl))
 *   )
 * ))
 * OOM test: Lisp 303, cfunc 326
 */
fe_Object* cfunc_map(fe_Context *ctx, fe_Object *arg)
{
    float x = fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    float from_min = fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    float from_max = fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    float to_min = fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    float to_max = fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    return fe_number(ctx,
        (x - from_min) * (to_max - to_min) / (from_max - from_min) + to_min
    );
}


/**
 * Round returns the closest integer to x,
 * rounding to even when x is halfway between two integers.
 */
fe_Object* cfunc_round(fe_Context *ctx, fe_Object *arg)
{
    float x = fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    // roundf would round half away from zero
    // FE_TONEAREST should be the default rounding mode
    x = nearbyintf(x);
    return fe_number(ctx, x);
}


/**
 * C implementation of:
 *
 * (= lim (fn (x l h)
 *   (if
 *     (< x l) l
 *     (< h x) h
 *     x
 *   )
 * ))
 */
fe_Object* cfunc_lim(fe_Context *ctx, fe_Object *arg)
{
    fe_Number x = fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    fe_Number l = fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    fe_Number h = fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    if (x < l) return fe_number(ctx, l);
    if (x > h) return fe_number(ctx, h);
    return fe_number(ctx, x);
}

fe_Object* cfunc_pow(fe_Context *ctx, fe_Object *arg)
{
    fe_Number x = fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    fe_Number y = fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    // this uses 1076 bytes of flash less than powf
    return fe_number(ctx, expf(y * logf(x)));
}


/**
 * Always returns nil.
 * Useful as a default for callback functions.
 */
fe_Object* cfunc_empty(fe_Context *ctx, fe_Object *arg)
{
    return fe_bool(ctx, 0);
}


/**
 * fe_ReadFn that can execute fe code from fe_str_t.
 */
char fe_read_str(fe_Context *ctx, void *udata)
{
    (void)ctx;
    fe_str_t * lstr = (fe_str_t *)udata;

    if (lstr->i >= lstr->length) return '\0';
    return lstr->str[lstr->i++];
}


/**
 * \brief Add all fe_utils functions to a fe_Context.
 */
void fe_utils_init(fe_Context *ctx)
{
    fe_set(ctx, fe_symbol(ctx, "rem"), fe_cfunc(ctx, cfunc_rem));
    fe_set(ctx, fe_symbol(ctx, "round"), fe_cfunc(ctx, cfunc_round));
    fe_set(ctx, fe_symbol(ctx, "map"), fe_cfunc(ctx, cfunc_map));
    fe_set(ctx, fe_symbol(ctx, "lim"), fe_cfunc(ctx, cfunc_lim));
    fe_set(ctx, fe_symbol(ctx, "pow"), fe_cfunc(ctx, cfunc_pow));
}
