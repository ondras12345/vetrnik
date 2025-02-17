#include "hal.h"
#include "settings.h"
#include "power_board.h"
#include "control.h"
#include "pump.h"
#include "stats.h"
#include "display.h"
#include "sensor_wind.h"
#include <Arduino.h>


static power_board_status_t pwr_get_status()
{
    return power_board_status;
}


static void pwr_reset()
{
    power_board_command(PCOMMAND_RESET);
}


static void pwr_test_WDT()
{
    power_board_command(PCOMMAND_WDT_TEST);
}


static stats_t stats_get()
{
    return stats;
}


static int out_get_pin(digital_output_t out)
{
    switch (out)
    {
        case OUT_LED_BLUE:
            return LED_BUILTIN;
        case OUT_LED_RED:
            return PIN_LED;
        case OUT_REL2:
            return PIN_REL2;
        default:
            return -1;
    }
}


static void out_set(digital_output_t out, bool s)
{
    switch (out)
    {
        case OUT_PUMP:
            pump_set(s);
            break;
        default:
            int pin = out_get_pin(out);
            if (pin < 0) return;
            digitalWrite(pin, s);
            break;
    }
}


static bool out_get(digital_output_t out)
{
    switch (out)
    {
        case OUT_PUMP:
            return pump_get();
        default:
            int pin = out_get_pin(out);
            if (pin < 0) return false;
            return digitalRead(pin);
    }
}


wt_hal_t wt_hal = {
    .pwr_get_status     = pwr_get_status,
    .pwr_set_duty       = power_board_set_duty,
    .pwr_set_mode       = power_board_set_mode,
    .pwr_set_sw_enable  = power_board_set_software_enable,
    .pwr_clear_errors   = power_board_clear_errors,
    .pwr_reset          = pwr_reset,
    .pwr_test_WDT       = pwr_test_WDT,
    .pwr_REL_write      = power_board_REL_write,
    .pwr_REL_read       = power_board_REL_read,

    .ctrl_set_strategy  = control_set_strategy,
    .ctrl_set_strategy_str = control_set_strategy,  // TODO common implementation in wt_hal
    .ctrl_get_strategy  = control_get_strategy,
    .ctrl_contactor_set = control_contactor_set,
    .ctrl_contactor_get = control_contactor_get,

    .pump_set = pump_set,
    .pump_get = pump_get,

    .stats_get = stats_get,

    .out_set = out_set,
    .out_get = out_get,
    .out_validate = wt_hal_out_validate,

    .vwind = sensor_wind_read,
};


lcd_hal_t lcd_hal = {
    .commit         = display_commit,
    .print          = display_print,
    .set_cursor     = display_set_cursor,
    .backlight_set  = display_backlight_set,
    .backlight_get  = display_backlight_get,
    .get_cols       = display_get_cols,
    .get_rows       = display_get_rows,
};
