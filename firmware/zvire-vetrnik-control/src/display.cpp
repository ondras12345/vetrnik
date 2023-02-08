#include "display.h"
#include <Arduino.h>
#include <LiquidCrystal_PCF8574.h>
#include "lisp.h"

LiquidCrystal_PCF8574 lcd(0x27);

static char display_buf[DISPLAY_COLS+1] = {0};
static uint8_t buf_index = 0;

static void clear_buf()
{
    memset(display_buf, ' ', DISPLAY_COLS);
    display_buf[DISPLAY_COLS] = '\0';
    buf_index = 0;
}


void display_init()
{
    Wire.begin();
    // TODO ping and abort if missing??
    lcd.begin(DISPLAY_COLS, DISPLAY_ROWS);
    lcd.setBacklight(255);
    clear_buf();
    lcd.clear();
    lcd.home();
    lcd.print("vetrnik-control");
}


void display_init_lisp()
{
    lisp_empty_func("disp");
}


void display_loop()
{
    static unsigned long last_ms = 0;
    unsigned long now = millis();
    if (now - last_ms >= 500UL)
    {
        last_ms = now;
        lisp_run_blind("(disp)");
    }
}


bool display_commit(uint8_t row)
{
    if (row >= DISPLAY_ROWS) return false;
    lcd.setCursor(0, row);
    lcd.print(display_buf);
    clear_buf();
    return true;
}


void display_print(const char * str)
{
    size_t len = strlen(str);
    uint8_t copy = DISPLAY_COLS - buf_index;
    if (len < copy) copy = len;
    memcpy(display_buf + buf_index, str, copy);
    buf_index += len;
}


bool display_set_cursor(uint8_t col)
{
    if (col >= DISPLAY_COLS) return false;
    buf_index = col;
    return true;
}


void display_backlight(bool state)
{
    lcd.setBacklight(state ? 255 : 0);
}
