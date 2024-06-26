#include "lisp.h"
#include "power_board.h"
#include "debug.h"
#include "display.h"
#include "stats.h"
#include "control.h"
#include "settings.h"
#include "sensor_DS18B20.h"
#include "pump.h"
#include "flash_tools.h"
#include <Arduino.h>

extern "C" {
#include <fe.h>
}
#include <fe_utils.h>
#include <setjmp.h>


// TODO expose millis ?? - no, float does not have enough resolution
// TODO timers (schedule LCD backlight off & similar)


static jmp_buf error_jmp;
// fe requires the buffer to be 32-bit aligned.
// I'm not sure if this is done by default on this platform, added
// __attribute__ aligned just to be sure.
static uint8_t lisp_buf[16*1024] __attribute__((aligned(4)));

static Print * error_print = nullptr;

/// Set to true in onerror. If you want to use it, you must set it to false
/// before you start.
static bool error_occured = false;

static void onerror(fe_Context *ctx, const char *msg, fe_Object *cl)
{
    if (error_print != nullptr)
    {
        error_print->printf("lisp error: %s\r\n", msg);

        // Stack trace
        for (; !fe_isnil(ctx, cl); cl = fe_cdr(ctx, cl))
        {
            char buf[64];
            fe_tostring(ctx, fe_car(ctx, cl), buf, sizeof(buf));
            error_print->printf("=> %s\r\n", buf);
        }
    }
    error_occured = true;
    longjmp(error_jmp, -1);
}


static void lisp_write_Print(fe_Context *ctx, void *udata, char chr)
{
    (void)ctx;
    (static_cast<Print *>(udata))->write(chr);
}


/**
 * call with (nullptr, nullptr) before reading new file
 */
static char lisp_read_file(fe_Context *ctx, void *udata)
{
    (void)ctx;
    static uint8_t buf[256];
    static size_t size = sizeof buf;
    static size_t i = size;

    if (udata == nullptr)
    {
        size = sizeof buf;
        i = size;
        return '\0';
    }

    if (i >= size)
    {
        SerialFlashFile * f = static_cast<SerialFlashFile *>(udata);
        size = f->read(buf, sizeof buf);
        if (size == 0) return '\0';  // EOF
        i = 0;
    }
    uint8_t c = buf[i++];
    if (c == 0xFF) return '\0';  // empty section of the file
    return c;
}


/**
 * Set LCD cursor position to specified column
 */
static fe_Object* cfunc_lcd_setc(fe_Context *ctx, fe_Object *arg)
{
    uint8_t col = (uint8_t)fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    if (!display_set_cursor(col))
    {
        fe_error(ctx, "invalid lcd col");
    }
    return fe_bool(ctx, 0);  // nil
}


/**
 * Write LCD buffer to specified line.
 */
static fe_Object* cfunc_lcd_write(fe_Context *ctx, fe_Object *arg)
{
    uint8_t row = (uint8_t)fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    if (!display_commit(row))
    {
        fe_error(ctx, "invalid lcd row");
    }
    return fe_bool(ctx, 0);
}


/**
 * Print string to lcd buffer
 */
static fe_Object* cfunc_lcd_str(fe_Context *ctx, fe_Object *arg)
{
    char buf[DISPLAY_COLS+1];
    fe_tostring(ctx, fe_nextarg(ctx, &arg), buf, sizeof buf);
    display_print(buf);
    return fe_bool(ctx, 0);
}


/**
 * Print number to lcd buffer
 * (lcd_num number align precision)
 */
static fe_Object* cfunc_lcd_num(fe_Context *ctx, fe_Object *arg)
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
    display_print(buf);
    return fe_bool(ctx, 0);
}


static fe_Object* cfunc_lcd_backlight(fe_Context *ctx, fe_Object *arg)
{
    if (fe_isnil(ctx, arg))
    {
        // called with no argument --> getter
        return fe_bool(ctx, display_backlight_get());
    }

    // called with an argument --> setter
    bool state = !fe_isnil(ctx, fe_nextarg(ctx, &arg));
    display_backlight_set(state);
    return fe_bool(ctx, state);
}


static fe_Object* cfunc_stats(fe_Context *ctx, fe_Object *arg)
{
    char name[32];
    fe_tostring(ctx, fe_nextarg(ctx, &arg), name, sizeof name);
    if (strcmp(name, "energy") == 0) return fe_number(ctx, stats.energy * 0.001);
    else
    {
        fe_error(ctx, "invalid stat name");
        // this should never happen, fe_error either exits or longjmps
        return fe_bool(ctx, 0);
    }
}


static fe_Object* cfunc_power_get(fe_Context *ctx, fe_Object *arg)
{
    char name[32];
    fe_tostring(ctx, fe_nextarg(ctx, &arg), name, sizeof name);

    if (strcmp(name, "valid") == 0) return fe_bool(ctx, power_board_status.valid);
#define pbstateBN(n, var) \
    else if (strcmp(name, n) == 0) return fe_bool(ctx, power_board_status.var);
#define pbstateB(n) pbstateBN(#n, n)
#define pbstateCN(n, var, conv) \
    else if (strcmp(name, n) == 0) return fe_number(ctx, power_board_status.var * conv);
#define pbstateC(n, conv) pbstateCN(#n, n, conv)
#define pbstate(n) pbstateC(n, 1)
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
        uint8_t result = power_board_REL_read(pin);
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


static fe_Object* cfunc_power_set(fe_Context *ctx, fe_Object *arg)
{
    char name[10];
    fe_tostring(ctx, fe_nextarg(ctx, &arg), name, sizeof name);

    if (strcmp(name, "duty") == 0)
    {
        int duty = (int)fe_tonumber(ctx, fe_nextarg(ctx, &arg));
        if (duty < 0 || duty > 255)
        {
            fe_error(ctx, "duty must be 0-255");
            return nullptr;
        }
        power_board_set_duty(duty);
    }
    else if (strcmp(name, "mode") == 0)
    {
        int mode = (int)fe_tonumber(ctx, fe_nextarg(ctx, &arg));

        size_t mode_max = 0;
        for (; power_board_modes[mode_max] != nullptr; mode_max++);
        mode_max--;

        if (mode < 0 || (size_t)mode > mode_max)
        {
            fe_error(ctx, "invalid mode");
            return nullptr;
        }
        power_board_set_mode((power_board_mode_t)mode);
    }
    else if (strcmp(name, "REL") == 0)
    {
        int pin = (int)fe_tonumber(ctx, fe_nextarg(ctx, &arg));
        bool state = !fe_isnil(ctx, fe_nextarg(ctx, &arg));

        if (!power_board_REL_write(pin, state))
            fe_error(ctx, "invalid REL pin");
    }
    else if (strcmp(name, "sw_enable") == 0)
    {
        bool value = !fe_isnil(ctx, fe_nextarg(ctx, &arg));
        power_board_set_software_enable(value);
    }
    // there should be no need to clear_errors or execute other commands
    else
    {
        fe_error(ctx, "invalid pwrs command");
        return fe_bool(ctx, 0); // this should never happen, fe_error either exits or longjmps
    }

    return fe_bool(ctx, 0);  // nil
}


static fe_Object* cfunc_control_get(fe_Context *ctx, fe_Object *arg)
{
    char name[32];
    fe_tostring(ctx, fe_nextarg(ctx, &arg), name, sizeof name);
    if (strcmp(name, "strategy") == 0)
    {
        return fe_string(ctx, control_strategies[control_get_strategy()]);
    }
    else if (strcmp(name, "contactor") == 0)
    {
        unsigned long cs = control_contactor_get();
        fe_Number r = -1;
        if (cs != (unsigned long)-1) r = cs;
        return fe_number(ctx, r);
    }
    else
    {
        fe_error(ctx, "invalid control param name");
        return fe_bool(ctx, 0); // this should never happen, fe_error either exits or longjmps
    }
}


static fe_Object* cfunc_control_set(fe_Context *ctx, fe_Object *arg)
{
    char name[32];
    fe_tostring(ctx, fe_nextarg(ctx, &arg), name, sizeof name);
    if (strcmp(name, "strategy") == 0)
    {
        char strategy_name[sizeof("control_shorted")+10];  // should be enough
        fe_tostring(ctx, fe_nextarg(ctx, &arg), strategy_name, sizeof strategy_name);
        if (!control_set_strategy(strategy_name))
        {
            fe_error(ctx, "invalid strategy");
            return nullptr;
        }
    }
    else if (strcmp(name, "contactor") == 0)
    {
        bool state = !fe_isnil(ctx, fe_nextarg(ctx, &arg));
        if (state) control_contactor_set();
    }
    else
    {
        fe_error(ctx, "invalid control param name");
    }
    return fe_bool(ctx, 0);
}


static fe_Object* cfunc_relay_get(fe_Context *ctx, fe_Object *arg)
{
    int relay_number = (int)fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    switch (relay_number)
    {
        // REL1 is currently used for pump, disable direct control
        //case 1:
        //    return fe_bool(ctx, digitalRead(PIN_REL1));
        case 2:
            return fe_bool(ctx, digitalRead(PIN_REL2));
        default:
            fe_error(ctx, "invalid relay number");
            return fe_bool(ctx, 0);
    }
}


static fe_Object* cfunc_relay_set(fe_Context *ctx, fe_Object *arg)
{
    int relay_number = (int)fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    bool state = !fe_isnil(ctx, fe_nextarg(ctx, &arg));

    switch (relay_number)
    {
        // REL1 is currently used for pump, disable direct control
        //case 1:
        //    digitalWrite(PIN_REL1, state);
        //    break;
        case 2:
            digitalWrite(PIN_REL2, state);
            break;
        default:
            fe_error(ctx, "invalid relay number");
            break;
    }

    return fe_bool(ctx, 0);
}


static fe_Object* cfunc_LED_get(fe_Context *ctx, fe_Object *arg)
{
    int LED_number = (int)fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    switch (LED_number)
    {
        case 1:
            return fe_bool(ctx, !digitalRead(LED_BUILTIN));
        case 2:
            return fe_bool(ctx, digitalRead(PIN_LED));
        default:
            fe_error(ctx, "invalid LED number");
            return fe_bool(ctx, 0);
    }
}


static fe_Object* cfunc_LED_set(fe_Context *ctx, fe_Object *arg)
{
    int LED_number = (int)fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    bool state = !fe_isnil(ctx, fe_nextarg(ctx, &arg));
    switch (LED_number)
    {
        case 1:
            digitalWrite(LED_BUILTIN, !state);
            break;
        case 2:
            digitalWrite(PIN_LED, state);
            break;
        default:
            fe_error(ctx, "invalid LED number");
            break;
    }
    return fe_bool(ctx, 0);
}


static fe_Object* cfunc_DS18B20(fe_Context *ctx, fe_Object *arg)
{
    int sensor_number = (int)fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    if (sensor_number >= SENSOR_DS18B20_COUNT || sensor_number < 0)
    {
        fe_error(ctx, "invalid DS18B20 number");
        return fe_bool(ctx, 0);
    }
    uint16_t reading = sensor_DS18B20_readings[sensor_number];
    if (reading == 0) return fe_bool(ctx, 0);
    return fe_number(ctx, reading / 100.0);
}


static fe_Object* cfunc_pump_get(fe_Context *ctx, fe_Object *arg)
{
    return fe_bool(ctx, pump_get());
}


static fe_Object* cfunc_pump_set(fe_Context *ctx, fe_Object *arg)
{
    bool state = !fe_isnil(ctx, fe_nextarg(ctx, &arg));
    pump_set(state);
    return fe_bool(ctx, 0);
}


static int gc;
static fe_Context *ctx;

void lisp_init()
{
    ctx = fe_open(lisp_buf, sizeof(lisp_buf));
    fe_handlers(ctx)->error = onerror;

    gc = fe_savegc(ctx);

    // TODO combine getters and setters to save LISP RAM
    // (lcdb already does it)
    fe_set(ctx, fe_symbol(ctx, "pwrg"), fe_cfunc(ctx, cfunc_power_get));
    fe_set(ctx, fe_symbol(ctx, "pwrs"), fe_cfunc(ctx, cfunc_power_set));
    fe_set(ctx, fe_symbol(ctx, "rem"), fe_cfunc(ctx, cfunc_rem));
    fe_set(ctx, fe_symbol(ctx, "round"), fe_cfunc(ctx, cfunc_round));
    fe_set(ctx, fe_symbol(ctx, "map"), fe_cfunc(ctx, cfunc_map));
    fe_set(ctx, fe_symbol(ctx, "lcdc"), fe_cfunc(ctx, cfunc_lcd_setc));
    fe_set(ctx, fe_symbol(ctx, "lcdw"), fe_cfunc(ctx, cfunc_lcd_write));
    fe_set(ctx, fe_symbol(ctx, "lcds"), fe_cfunc(ctx, cfunc_lcd_str));
    fe_set(ctx, fe_symbol(ctx, "lcdn"), fe_cfunc(ctx, cfunc_lcd_num));
    fe_set(ctx, fe_symbol(ctx, "lcdb"), fe_cfunc(ctx, cfunc_lcd_backlight));
    fe_set(ctx, fe_symbol(ctx, "stats"), fe_cfunc(ctx, cfunc_stats));
    fe_set(ctx, fe_symbol(ctx, "ctrlg"), fe_cfunc(ctx, cfunc_control_get));
    fe_set(ctx, fe_symbol(ctx, "ctrls"), fe_cfunc(ctx, cfunc_control_set));
    fe_set(ctx, fe_symbol(ctx, "relg"), fe_cfunc(ctx, cfunc_relay_get));
    fe_set(ctx, fe_symbol(ctx, "rels"), fe_cfunc(ctx, cfunc_relay_set));
    fe_set(ctx, fe_symbol(ctx, "ledg"), fe_cfunc(ctx, cfunc_LED_get));
    fe_set(ctx, fe_symbol(ctx, "leds"), fe_cfunc(ctx, cfunc_LED_set));
    fe_set(ctx, fe_symbol(ctx, "ds18"), fe_cfunc(ctx, cfunc_DS18B20));
    fe_set(ctx, fe_symbol(ctx, "pumpg"), fe_cfunc(ctx, cfunc_pump_get));
    fe_set(ctx, fe_symbol(ctx, "pumps"), fe_cfunc(ctx, cfunc_pump_set));

    // Add variables for power modes
    for (size_t i = 0; power_board_modes[i] != nullptr; i++)
    {
        char buff[32];
        snprintf(buff, sizeof buff, "pwr_%s", power_board_modes[i]);
        fe_set(ctx, fe_symbol(ctx, buff), fe_number(ctx, i));
    }

    fe_restoregc(ctx, gc);
}


void lisp_reinit()
{
    fe_close(ctx);
    lisp_init();
}


static fe_Object * lisp_execute(fe_ReadFn readfn, void * udata)
{
    bool jumped_in = false;
    setjmp(error_jmp);
    if (jumped_in)
    {
        fe_restoregc(ctx, gc);
        return nullptr;
    }
    jumped_in = true;  //cppcheck-suppress unreadVariable

    fe_Object *obj;
    obj = fe_read(ctx, readfn, udata);
    if (obj != nullptr)
    {
        obj = fe_eval(ctx, obj);
    }

    fe_restoregc(ctx, gc);
    return obj;
}


/**
 * Run lisp code and discard the output.
 * Errors will be printed out to INFO debug stream.
 *
 * @param code Array of characters without null terminator
 * @param length Length of code array
 * @return true on success, false if error occurred.
 */
bool lisp_run_blind(const char * code, size_t length)
{
    error_occured = false;
    error_print = INFO;
    fe_str_t fstr = { code, length, 0 };
    // don't care about the result, execute all root-level expressions
    while (lisp_execute(fe_read_str, &fstr) != nullptr);
    return !error_occured;
}


/**
 * Run lisp code and discard the output.
 * Errors will be printed out to INFO debug stream.
 *
 * @param code null-terminated string containing the code to run
 * @return true on success, false if error occurred.
 */
bool lisp_run_blind(const char * code)
{
    return lisp_run_blind(code, -1);
}


/**
 * Run lisp code from file and discard the output.
 * Errors will be printed out to INFO debug stream.
 *
 * The file can be arbitrary size. The Lisp program does not need to be
 * null-terminated -- 0xFF suffices as a terminator.
 *
 * @param filename file to read the code from
 * @param offset offset to seek to; first non-zero byte if offset=-1
 * @return true on success, false if error occurred.
 */
bool lisp_run_blind_file(const char * filename, uint32_t offset)
{
    error_occured = false;
    error_print = INFO;
    SerialFlashFile f = SerialFlash.open(filename);
    if (!f) return false;
    if (offset == (uint32_t)-1) offset = flash_find_byte(&f, 0x00, true) + 1;

    f.seek(offset);
    lisp_read_file(nullptr, nullptr);
    while (lisp_execute(lisp_read_file, &f) != nullptr);
    f.close();
    return !error_occured;
}


/**
 * Evaluate single lisp expression and print out the result to a Stream.
 *
 * @param code Array of characters without null terminator
 * @param length Length of code array
 * @param response Print to print the result to
 */
bool lisp_process(const char * code, size_t length, Print * response)
{
    error_print = response;

    fe_str_t fstr = { code, length, 0 };
    fe_Object * obj = lisp_execute(fe_read_str, &fstr);
    if (obj != nullptr)
    {
        fe_write(ctx, obj, lisp_write_Print, response, 0);
        return true;
    }
    return false;
}


/**
 * Evaluate single lisp expression and print out the result to a Stream.
 *
 * @param code null-terminated string containing the code to run
 * @param response Print to print the result to
 */
bool lisp_process(const char * code, Print * response)
{
    return lisp_process(code, -1, response);
}


/**
 * Register an empty function with no parameters.
 */
void lisp_empty_func(const char * name)
{
    fe_set(ctx, fe_symbol(ctx, name), fe_cfunc(ctx, cfunc_empty));
    fe_restoregc(ctx, gc);
}
