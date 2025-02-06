#include "wt_sil.h"
#include <string.h>
#include <wt_lisp.h>
#include <lcd_lisp.h>
#include <fe_utils.h>
#include <assert.h>
#include <unistd.h>
#include "hal.h"

void wt_sil_init(wt_sil_state_t * state)
{
    // sensible defaults
    memset(state, 0, sizeof *state);
    state->pwr_status.time = 0;
    state->pwr_status.mode = const_duty;
    state->pwr_status.duty = 0;
    state->pwr_status.OCP_max_duty = 255;
    state->pwr_status.enabled = (enabled_t){true, true, true};
    state->pwr_status.last5m = true;
    state->ctrl_strategy = control_lisp;

    sil_hal_init(state);

    fe_Context * ctx = fe_open(state->fe_buf, sizeof state->fe_buf);;
    state->fe_ctx = ctx;

    int gc = fe_savegc(ctx);

    fe_utils_init(ctx);
    wt_lisp_init(ctx, wt_hal);
    lcd_lisp_init(ctx, lcd_hal);
    fe_set(ctx, fe_symbol(ctx, "ds18"), fe_cfunc(ctx, cfunc_empty));  // TODO unimplemented for now - nil return should be interpreted as broken sensor

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
        perror("open failed");
        return 1;
    }

    fe_Object *obj;
    fe_Context * ctx = state->fe_ctx;
    int gc = fe_savegc(ctx);
    for (;;)
    {
        if (!(obj = fe_readfp(ctx, f))) break;
        fe_eval(ctx, obj);
        fe_restoregc(ctx, gc);
    }

    fclose(f);
    return 0;
}

/**
 * \brief Execute lisp code from a string and discard the result.
 * \a code can contain multiple top-level statements. In that case, they will be
 * executed in sequence.
 * \param code code to execute
 */
void wt_sil_run_str(wt_sil_state_t * state, const char * code)
{
    fe_str_t fstr = { code, -1, 0 };
    fe_Context * ctx = state->fe_ctx;
    int gc = fe_savegc(ctx);
    for (;;)
    {
        fe_Object *obj;
        if (!(obj = fe_read(ctx, fe_read_str, &fstr))) break;
        fe_eval(ctx, obj);
        fe_restoregc(ctx, gc);
    }
}


/**
 * \brief Simple controller function for use in Modelica SIL.
 * Uses static variables.
 * \param filename path to lisp file that should be sourced during first call
 * \param RPM turbine revolutions per minute
 * \param voltage DC voltage in volts
 * \param current DC current in amps
 * \param vwind velocity of the wind in meters per second
 * \return duty cycle 0...1
 */
double wt_sil_controller(const char * filename, double RPM, double voltage, double current, double vwind)
{
    static wt_sil_state_t state;
    static bool initialized = false;
    if (!initialized)
    {
        initialized = true;
        wt_sil_init(&state);
        if (wt_sil_run_file(&state, filename)) exit(EXIT_FAILURE);
    }

    assert(RPM >= 0);
    assert(voltage >= 0);
    assert(current >= 0);

    state.pwr_status.RPM = (RPM > UINT16_MAX) ? UINT16_MAX : RPM;
    voltage *= 10;
    state.pwr_status.voltage = (voltage > UINT16_MAX) ? UINT16_MAX : voltage;
    current *= 1e3;
    state.pwr_status.current = (current > UINT16_MAX) ? UINT16_MAX : current;

    state.pwr_status.time += 1;

    wt_sil_run_str(&state, "(ctrl)");

    uint8_t duty = state.pwr_status.duty;
    // vetrnik-power has a limited range of duty cycles
    if (duty > 247) duty = 247;
    if (duty < 4) duty = 0;
    return duty / 255.0;
}
