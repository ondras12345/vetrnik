#include "wt_hal.h"
#include <stddef.h>


#define X_STR(name, value) #name,
/**
 * Array with strings corresponding to control_strategy_t values.
 * Terminated by NULL.
 *
 * Useful for performing reverse lookup.
 */
const char * control_strategies[] = {
    CONTROL_STRATEGIES(X_STR)
    NULL
};
#undef X_STR


#define X_STR(name, value) #name,
/**
 * Array with strings corresponding to power_board_mode_t values.
 * Terminated by NULL.
 *
 * Should NOT be used for printing out states from power_board_status_t
 * as the power board might have newer firmware than this, and could thus know
 * more states (out-of-bounds).
 *
 * Useful for performing reverse lookup.
 */
const char * power_board_modes[] = {
    POWER_BOARD_MODES(X_STR)
    NULL
};
#undef X_STR


bool wt_hal_out_validate(int v)
{
    return (v > 0 && v < OUT_LAST_);
}
