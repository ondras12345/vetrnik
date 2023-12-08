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
    fe_str_t * lstr = static_cast<fe_str_t *>(udata);

    if (lstr->i >= lstr->length) return '\0';
    return lstr->str[lstr->i++];
}
