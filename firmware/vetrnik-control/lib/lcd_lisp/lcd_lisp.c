#include "lcd_lisp.h"

static lcd_hal_t lcd;


static fe_Object* cfunc_lcdc(fe_Context *ctx, fe_Object *arg)
{
    uint8_t col = (uint8_t)fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    if (!lcd.set_cursor(col)) fe_error(ctx, "invalid lcd col");
    return fe_bool(ctx, 0);  // nil
}


static fe_Object* cfunc_lcdw(fe_Context *ctx, fe_Object *arg)
{
    uint8_t row = (uint8_t)fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    if (!lcd.commit(row)) fe_error(ctx, "invalid lcd row");
    return fe_bool(ctx, 0);
}


static fe_Object* cfunc_lcds(fe_Context *ctx, fe_Object *arg)
{
    char buf[LCD_COLS_MAX+1];
    fe_tostring(ctx, fe_nextarg(ctx, &arg), buf, sizeof buf);
    lcd.print(buf);
    return fe_bool(ctx, 0);
}


static fe_Object* cfunc_lcdn(fe_Context *ctx, fe_Object *arg)
{
    float num = fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    uint8_t align = (uint8_t)fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    uint8_t precision = (uint8_t)fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    if (precision > 5) fe_error(ctx, "invalid precision");
    if (align > 5) fe_error(ctx, "invalid align");
    char form[
        1 /* % */ + 1 /* align */ + 1 /* . */ + 1 /* precision */ + 1 /* f */
        + 1 /* '\0' */
    ];
    snprintf(form, sizeof form, "%%%u.%uf",
             align+precision + ((precision > 0) ? 1U : 0U),
             precision);
    char buf[10];
    snprintf(buf, sizeof buf, form, num);
    lcd.print(buf);
    return fe_bool(ctx, 0);
}


static fe_Object* cfunc_lcdb(fe_Context *ctx, fe_Object *arg)
{
    if (fe_isnil(ctx, arg))
    {
        // called with no argument --> getter
        return fe_bool(ctx, lcd.backlight_get());
    }

    // called with an argument --> setter
    bool state = !fe_isnil(ctx, fe_nextarg(ctx, &arg));
    lcd.backlight_set(state);
    return fe_bool(ctx, state);
}




/**
 * \brief Add lcd_lisp functions to a fe_Context.
 * \a lcd is copied to a global variable, so only one lcd_hal
 * implementation per program is allowed.
 * \param ctx context to add the symbols to
 * \param hal display HAL implementation.
 */
void lcd_lisp_init(fe_Context *ctx, lcd_hal_t hal)
{
    lcd = hal;

    fe_set(ctx, fe_symbol(ctx, "lcdc"), fe_cfunc(ctx, cfunc_lcdc));
    fe_set(ctx, fe_symbol(ctx, "lcdw"), fe_cfunc(ctx, cfunc_lcdw));
    fe_set(ctx, fe_symbol(ctx, "lcds"), fe_cfunc(ctx, cfunc_lcds));
    fe_set(ctx, fe_symbol(ctx, "lcdn"), fe_cfunc(ctx, cfunc_lcdn));
    fe_set(ctx, fe_symbol(ctx, "lcdb"), fe_cfunc(ctx, cfunc_lcdb));
}
