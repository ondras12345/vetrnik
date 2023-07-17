#pragma once
extern "C" {
#include <fe.h>
}


fe_Object* cfunc_rem(fe_Context *ctx, fe_Object *arg);
fe_Object* cfunc_map(fe_Context *ctx, fe_Object *arg);
fe_Object* cfunc_round(fe_Context *ctx, fe_Object *arg);
fe_Object* cfunc_empty(fe_Context *ctx, fe_Object *arg);


/**
 * String that can be passed to fe_read_str.
 */
typedef struct {
    /**
     * Array of characters without null terminator.
     * Alternatively, a null-terminated string can be used.
     * If that is the case, set length to -1.
     */
    const char * str;
    /// Length of str array (if not null-terminated)
    size_t length;
    /// Index, used internally, initialize to 0.
    size_t i;
} fe_str_t;


char fe_read_str(fe_Context *ctx, void *udata);
