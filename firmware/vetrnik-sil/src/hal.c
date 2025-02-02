#include "wt_sil.h"
#include <lcd_hal.h>
#include <string.h>

static wt_sil_state_t * sil_state;


static power_board_status_t pwr_get_status()
{
    return sil_state->pwr_status;
}

static void pwr_set_duty(uint8_t d)
{
    sil_state->pwr_status.duty = d;
}

static void pwr_set_mode(power_board_mode_t m)
{
    sil_state->pwr_status.mode = m;
}

static void pwr_set_sw_enable(bool s)
{
    sil_state->pwr_status.enabled.software = s;
}

static void empty()
{
}

static bool pwr_REL_write(uint8_t pin, bool value)
{
    if (pin < 1 || pin > POWER_REL_COUNT) return false;
    sil_state->pwr_REL[pin] = value;
    return true;
}

static uint8_t pwr_REL_read(uint8_t pin)
{
    if (pin < 1 || pin > POWER_REL_COUNT) return -1;
    return sil_state->pwr_REL[pin] ? 1 : 0;
}


static void ctrl_set_strategy(control_strategy_t s)
{
    if (s == control_shorted) sil_state->ctrl_contactor_state = false;
    sil_state->ctrl_strategy = s;
}

static bool ctrl_set_strategy_str(const char * str)
{
    // TODO common
}

static control_strategy_t ctrl_get_strategy()
{
    return sil_state->ctrl_strategy;
}

static void ctrl_contactor_set()
{
    sil_state->ctrl_contactor_state = true;
}

static unsigned long ctrl_contactor_get()
{
    return sil_state->ctrl_contactor_state ? 1 : -1;
}

static void pump_set(bool s)
{
    sil_state->pump = s;
}

static bool pump_get()
{
    return sil_state->pump;
}

static stats_t stats_get()
{
    return sil_state->stats;
}


static void out_set(digital_output_t out, bool s)
{
    if (out >= sizeof sil_state->outputs) return;
    sil_state->outputs[out] = s;
}

static bool out_get(digital_output_t out)
{
    if (out >= sizeof sil_state->outputs) return false;
    return sil_state->outputs[out];
}


static void clear_buf()
{
    memset(sil_state->lcd_buf, ' ', LCD_COLS);
    sil_state->lcd_buf[LCD_COLS] = '\0';
    sil_state->lcd_buf_index = 0;
}


static bool display_commit(uint8_t row)
{
    if (row >= LCD_ROWS) return false;
    memcpy(sil_state->lcd[row], sil_state->lcd_buf, sizeof sil_state->lcd[row]);
    clear_buf();
    return true;
}

static void display_print(const char * str)
{
    size_t len = strlen(str);
    uint8_t copy = LCD_COLS - sil_state->lcd_buf_index;
    if (len < copy) copy = len;
    memcpy(sil_state->lcd_buf + sil_state->lcd_buf_index, str, copy);
    sil_state->lcd_buf_index += len;
}


static bool display_set_cursor(uint8_t col)
{
    if (col >= LCD_COLS) return false;
    sil_state->lcd_buf_index = col;
    return true;
}


static void display_backlight_set(bool state)
{
    sil_state->lcd_backlight = state;
}


static bool display_backlight_get()
{
    return sil_state->lcd_backlight;
}


static uint8_t display_get_cols()
{
    return LCD_COLS;
}


static uint8_t display_get_rows()
{
    return LCD_ROWS;
}


wt_hal_t wt_hal = {
    .pwr_get_status     = pwr_get_status,
    .pwr_set_duty       = pwr_set_duty,
    .pwr_set_mode       = pwr_set_mode,
    .pwr_set_sw_enable  = pwr_set_sw_enable,
    .pwr_clear_errors   = empty,
    .pwr_reset          = empty,
    .pwr_test_WDT       = empty,
    .pwr_REL_write      = pwr_REL_write,
    .pwr_REL_read       = pwr_REL_read,

    .ctrl_set_strategy  = ctrl_set_strategy,
    .ctrl_set_strategy_str = ctrl_set_strategy_str,  // TODO common implementation in wt_hal
    .ctrl_get_strategy  = ctrl_get_strategy,
    .ctrl_contactor_set = ctrl_contactor_set,
    .ctrl_contactor_get = ctrl_contactor_get,

    .pump_set = pump_set,
    .pump_get = pump_get,

    .stats_get = stats_get,

    .out_set = out_set,
    .out_get = out_get,
    .out_validate = wt_hal_out_validate,
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


/**
 * \brief Initialize the HAL.
 * Warning: the value of \a s is copied to a global variable.
 * \param s SIL state the HAL should work with.
 */
void sil_hal_init(wt_sil_state_t * s)
{
    sil_state = s;
}
