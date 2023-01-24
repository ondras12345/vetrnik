#include "control.h"
#include "settings.h"
#include "power_board.h"

#ifdef LISP_REPL
#include "lisp.h"
#endif


/**
 * Must be called after lisp is initialized.
 */
void control_init()
{
    // Add an empty control function
    lisp_run_blind("(= control (fn () ))");
}


/**
 * To be called when a new state object is received from power board.
 */
void control_new_state()
{
#ifdef LISP_REPL
    lisp_run_blind("(control)");  // TODO switch to other strategy on error
#endif
}
