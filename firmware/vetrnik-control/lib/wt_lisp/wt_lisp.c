#include "wt_lisp.h"
#include <fe_utils.h>
#include <stddef.h>
#include <string.h>

static wt_hal_t wt;


static fe_Object* cfunc_pwrg(fe_Context *ctx, fe_Object *arg)
{
    char name[32];
    fe_tostring(ctx, fe_nextarg(ctx, &arg), name, sizeof name);

    power_board_status_t s = wt.pwr_get_status();

    if (0)
        ;
#define pbstateBN(n, var) \
    else if (strcmp(name, n) == 0) return fe_bool(ctx, s.var);
#define pbstateB(n) pbstateBN(#n, n)
#define pbstateCN(n, var, conv) \
    else if (strcmp(name, n) == 0) return fe_number(ctx, s.var * conv);
#define pbstateC(n, conv) pbstateCN(#n, n, conv)
#define pbstate(n) pbstateC(n, 1)
    pbstate(valid)
    pbstate(time)
    pbstate(mode)
    pbstate(duty)
    pbstate(OCP_max_duty)
    pbstate(RPM)
    pbstateC(voltage, 0.1)
    pbstateC(current, 0.001)
    pbstateBN("hw_enable", enabled.hardware)
    pbstateBN("sw_enable", enabled.software)
    pbstateBN("enabled", enabled.overall)
    pbstateB(emergency)
    pbstateC(temperature_heatsink, 0.1)
    pbstateC(temperature_rectifier, 0.1)
    pbstate(fan)
    pbstate(error_count)
    pbstateB(last5m)
#undef pbstate
#undef pbstateC
#undef pbstateCN
#undef pbstateB
#undef pbstateBN
    else if (strcmp(name, "REL") == 0)
    {
        int pin = (int)fe_tonumber(ctx, fe_nextarg(ctx, &arg));
        uint8_t result = wt.pwr_REL_read(pin);
        if (result == (uint8_t)-1) fe_error(ctx, "invalid REL pin");
        return fe_bool(ctx, result);
    }
    else
    {
        fe_error(ctx, "invalid power state name");
        // this should never happen, fe_error either exits or longjmps
        return fe_bool(ctx, 0);
    }
}


static fe_Object* cfunc_pwrs(fe_Context *ctx, fe_Object *arg)
{
    char name[10];
    fe_tostring(ctx, fe_nextarg(ctx, &arg), name, sizeof name);

    if (strcmp(name, "duty") == 0)
    {
        int duty = (int)fe_tonumber(ctx, fe_nextarg(ctx, &arg));
        if (duty < 0 || duty > 255) fe_error(ctx, "duty must be 0-255");
        wt.pwr_set_duty(duty);
    }
    else if (strcmp(name, "mode") == 0)
    {
        int mode = (int)fe_tonumber(ctx, fe_nextarg(ctx, &arg));

        size_t mode_max = 0;
        for (; power_board_modes[mode_max] != NULL; mode_max++);
        mode_max--;

        if (mode < 0 || (size_t)mode > mode_max) fe_error(ctx, "invalid mode");
        wt.pwr_set_mode((power_board_mode_t)mode);
    }
    else if (strcmp(name, "REL") == 0)
    {
        int pin = (int)fe_tonumber(ctx, fe_nextarg(ctx, &arg));
        bool state = !fe_isnil(ctx, fe_nextarg(ctx, &arg));

        if (!wt.pwr_REL_write(pin, state))
            fe_error(ctx, "invalid REL pin");
    }
    else if (strcmp(name, "sw_enable") == 0)
    {
        bool value = !fe_isnil(ctx, fe_nextarg(ctx, &arg));
        wt.pwr_set_sw_enable(value);
    }
    // there should be no need to clear_errors or execute other commands
    else fe_error(ctx, "invalid pwrs command");

    return fe_bool(ctx, 0);  // nil
}


static fe_Object* cfunc_stats(fe_Context *ctx, fe_Object *arg)
{
    char name[32];
    fe_tostring(ctx, fe_nextarg(ctx, &arg), name, sizeof name);
    stats_t s = wt.stats_get();
    if (strcmp(name, "energy") == 0) return fe_number(ctx, s.energy * 0.001);
    else
    {
        fe_error(ctx, "invalid stat name");
        return NULL;  // suppress compiler warning, this will never happen
    }
}


static fe_Object* cfunc_ctrlg(fe_Context *ctx, fe_Object *arg)
{
    char name[32];
    fe_tostring(ctx, fe_nextarg(ctx, &arg), name, sizeof name);
    if (strcmp(name, "strategy") == 0)
    {
        return fe_string(ctx, control_strategies[wt.ctrl_get_strategy()]);
    }
    else if (strcmp(name, "contactor") == 0)
    {
        unsigned long cs = wt.ctrl_contactor_get();
        fe_Number r = -1;
        if (cs != (unsigned long)-1) r = cs;
        return fe_number(ctx, r);
    }
    else if (strcmp(name, "vwind") == 0)
    {
        fe_Number vwind = wt.vwind();
        if (vwind != vwind) return fe_bool(ctx, 0);  // translate NaN to nil
        return fe_number(ctx, vwind);
    }
    else
    {
        fe_error(ctx, "invalid ctrl param name");
        return NULL;  // suppress compiler warning, this will never happen
    }
}


static fe_Object* cfunc_ctrls(fe_Context *ctx, fe_Object *arg)
{
    char name[32];
    fe_tostring(ctx, fe_nextarg(ctx, &arg), name, sizeof name);
    if (strcmp(name, "strategy") == 0)
    {
        char strategy_name[sizeof("control_shorted")+10];  // should be enough
        fe_tostring(ctx, fe_nextarg(ctx, &arg), strategy_name, sizeof strategy_name);
        if (!wt.ctrl_set_strategy_str(strategy_name)) fe_error(ctx, "invalid strategy");
    }
    else if (strcmp(name, "contactor") == 0)
    {
        bool state = !fe_isnil(ctx, fe_nextarg(ctx, &arg));
        if (state) wt.ctrl_contactor_set();
    }
    else fe_error(ctx, "invalid ctrl param name");

    return fe_bool(ctx, 0);  // nil
}


static fe_Object* cfunc_out(fe_Context *ctx, fe_Object *arg)
{
    char name[16];
    fe_tostring(ctx, fe_nextarg(ctx, &arg), name, sizeof name);

    int out_number = -1;

    for (size_t i = 0; digital_output_names[i] != NULL; i++)
    {
        if (strcmp(name, digital_output_names[i]) == 0)
        {
            out_number = i;
            break;
        }
    }
    if (!wt.out_validate(out_number)) fe_error(ctx, "invalid out name");

    bool state;
    bool setter = false;

    if (!fe_isnil(ctx, arg))
    {
        // called with more than one argument --> setter
        setter = true;
        state = !fe_isnil(ctx, fe_nextarg(ctx, &arg));
    }

    if (setter) wt.out_set(out_number, state);
    else state = wt.out_get(out_number);

    return fe_bool(ctx, state);
}


/**
 * \brief Add wt_lisp functions to a fe_Context.
 * \a wt is copied to a global variable, so only one wt_hal
 * implementation per program is allowed.
 * \param ctx context to add the symbols to
 * \param hal wind turbine hal implementation.
 */
void wt_lisp_init(fe_Context *ctx, wt_hal_t hal)
{
    wt = hal;

    fe_set(ctx, fe_symbol(ctx, "pwrg"), fe_cfunc(ctx, cfunc_pwrg));
    fe_set(ctx, fe_symbol(ctx, "pwrs"), fe_cfunc(ctx, cfunc_pwrs));
    fe_set(ctx, fe_symbol(ctx, "stats"), fe_cfunc(ctx, cfunc_stats));
    fe_set(ctx, fe_symbol(ctx, "ctrlg"), fe_cfunc(ctx, cfunc_ctrlg));
    fe_set(ctx, fe_symbol(ctx, "ctrls"), fe_cfunc(ctx, cfunc_ctrls));
    fe_set(ctx, fe_symbol(ctx, "out"), fe_cfunc(ctx, cfunc_out));

    // Add variables for power modes
    for (size_t i = 0; power_board_modes[i] != NULL; i++)
    {
        char buf[32];
        snprintf(buf, sizeof buf, "pwr_%s", power_board_modes[i]);
        fe_set(ctx, fe_symbol(ctx, buf), fe_number(ctx, i));
    }
}
