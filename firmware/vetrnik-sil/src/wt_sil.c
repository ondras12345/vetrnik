#include "wt_sil.h"
#include <string.h>
#include <wt_lisp.h>
#include <lcd_lisp.h>
#include <fe_utils.h>
#include "hal.h"

void wt_sil_init(wt_sil_state_t * state)
{
    memset(state, 0, sizeof *state);
    sil_hal_init(state);

    fe_Context * ctx = fe_open(state->fe_buf, sizeof state->fe_buf);;
    state->fe_ctx = ctx;

    int gc = fe_savegc(ctx);

    fe_utils_init(ctx);
    wt_lisp_init(ctx, wt_hal);
    lcd_lisp_init(ctx, lcd_hal);
    //fe_set(ctx, fe_symbol(ctx, "ds18"), fe_cfunc(ctx, cfunc_DS18B20));  // TODO

    fe_restoregc(ctx, gc);
}


void wt_sil_close(wt_sil_state_t * state)
{
    fe_close(state->fe_ctx);
}


/// Start a LISP read-eval-print-loop on stdin/stdout
void wt_sil_repl(wt_sil_state_t * state)
{
    int gc;
    fe_Object *obj;
    fe_Context * ctx = state->fe_ctx;

    gc = fe_savegc(ctx);
    for (;;)
    {
        fe_restoregc(ctx, gc);
        printf("> ");
        if (!(obj = fe_readfp(ctx, stdin))) break;
        obj = fe_eval(ctx, obj);
        fe_writefp(ctx, obj, stdout);
        printf("\n");
    }
}


/// Execute lisp code from a file
/// \return 0 on success, nonzero on failure
int wt_sil_run_file(wt_sil_state_t * state, const char * filename)
{
    FILE * f = fopen(filename, "r");
    if (!f)
    {
        fprintf(stderr, "failed to open file %s\n", filename);
        return 1;
    }

    int gc;
    fe_Object *obj;
    fe_Context * ctx = state->fe_ctx;
    for (;;)
    {
        fe_restoregc(ctx, gc);
        if (!(obj = fe_readfp(ctx, f))) break;
        fe_eval(ctx, obj);
    }

    fclose(f);
    return 0;
}
