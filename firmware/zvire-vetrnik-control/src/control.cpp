#include "control.h"
#include "settings.h"
#include "power_board.h"
#include "lisp.h"
#include "debug.h"

static control_strategy_t strategy = control_shorted;


#define X_STR(name, value) #name,
/**
 * Array with strings corresponding to control_strategy_t values.
 * Terminated by nullptr.
 *
 * Useful for performing reverse lookup.
 */
const char * control_strategies[] = {
    CONTROL_STRATEGIES(X_STR)
    nullptr
};
#undef X_STR


void control_init()
{
    control_set_strategy(control_shorted);
}


/**
 * Must be called whenever lisp is (re)initialized.
 */
void control_init_lisp()
{
    // Add an empty control function
    lisp_run_blind("(= ctrl (fn () ))");
}


/**
 * To be called when a new state object is received from power board.
 */
void control_new_state()
{
    switch (strategy)
    {
        case control_shorted:
            if (power_board_status.mode != shorted)
                power_board_set_mode(shorted);
            break;

        case control_lisp:
            bool success = lisp_run_blind("(ctrl)");
            if (!success)
            {
                INFO.println("error in control_lisp, switching to control_shorted");
                control_set_strategy(control_shorted);
            }
            break;
    }
}


void control_set_strategy(control_strategy_t s)
{
    switch (s)
    {
        case control_shorted:
            power_board_set_duty(0);
            power_board_set_mode(shorted);
            break;
    }
    strategy = s;
}


control_strategy_t control_get_strategy()
{
    return strategy;
}
