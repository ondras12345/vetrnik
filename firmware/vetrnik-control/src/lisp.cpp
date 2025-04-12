#include "lisp.h"
#include "debug.h"
#include "settings.h"
#include "sensor_DS18B20.h"
#include "mqtt.h"
#include "flash_tools.h"

#include "hal.h"
#include <fe_utils.h>
#include <wt_lisp.h>
#include <lcd_lisp.h>
#include <setjmp.h>


// TODO expose millis ?? - no, float does not have enough resolution
// TODO timers (schedule LCD backlight off & similar)


static jmp_buf error_jmp;
// fe requires the buffer to be 32-bit aligned.
// I'm not sure if this is done by default on this platform, added
// __attribute__ aligned just to be sure.
static uint8_t lisp_buf[24*1024] __attribute__((aligned(4)));

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


static fe_Object* cfunc_DS18B20(fe_Context *ctx, fe_Object *arg)
{
    int sensor_number = (int)fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    if (sensor_number >= SENSOR_DS18B20_COUNT || sensor_number < 0)
        fe_error(ctx, "invalid DS18B20 number");
    uint16_t reading = sensor_DS18B20_readings[sensor_number];
    if (reading == 0) return fe_bool(ctx, 0);
    return fe_number(ctx, reading / 100.0);
}


static fe_Object* cfunc_ethrst(fe_Context *ctx, fe_Object *arg)
{
    return fe_number(ctx, MQTT_reinit());
}


static int gc;
static fe_Context *ctx;

void lisp_init()
{
    ctx = fe_open(lisp_buf, sizeof(lisp_buf));
    fe_handlers(ctx)->error = onerror;

    gc = fe_savegc(ctx);

    fe_utils_init(ctx);
    wt_lisp_init(ctx, wt_hal);
    lcd_lisp_init(ctx, lcd_hal);

    fe_set(ctx, fe_symbol(ctx, "ethrst"), fe_cfunc(ctx, cfunc_ethrst));
    fe_set(ctx, fe_symbol(ctx, "ds18"), fe_cfunc(ctx, cfunc_DS18B20));

    lisp_empty_func("ctrl");
    lisp_empty_func("disp");

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
