#include "lisp.h"
#include "power_board.h"

extern "C" {
#include <fe.h>
}
#include <setjmp.h>


// TODO extra lib for generic cfuncs (native pio test)
// TODO expose millis ??


static jmp_buf error_jmp;
static char lisp_buf[8*1024];

static Stream * error_stream = nullptr;

/// Set to true in onerror. If you want to use it, you must set it to false
/// before you start.
static bool error_occured = false;

static void onerror(fe_Context *ctx, const char *msg, fe_Object *cl)
{
    if (error_stream != nullptr)
    {
        error_stream->printf("lisp error: %s\r\n", msg);

        // Stack trace
        // There doesn't seem to be an isnil() function,
        // but fe_cdr returns its argument when it's nil.
        for (; cl != fe_cdr(ctx, cl); cl = fe_cdr(ctx, cl))
        {
            char buf[64];
            fe_tostring(ctx, fe_car(ctx, cl), buf, sizeof(buf));
            error_stream->printf("=> %s\r\n", buf);
        }
    }
    error_occured = true;
    longjmp(error_jmp, -1);
}


static void lisp_write_Stream(fe_Context *ctx, void *udata, char chr)
{
    (void)ctx;
    (static_cast<Stream *>(udata))->write(chr);
}


typedef struct lisp_str {
    const char * str;
    size_t length;
    size_t i;
} lisp_str_t;


static char lisp_read_str(fe_Context *ctx, void *udata)
{
    (void)ctx;
    lisp_str_t * lstr = static_cast<lisp_str_t *>(udata);

    if (lstr->i >= lstr->length) return '\0';
    return lstr->str[lstr->i++];
}


/**
 * Remainder function
 *
 * rem performs the operation truncate on number and divisor and returns the
 * remainder of the truncate operation.
 *
 * https://stackoverflow.com/questions/5706398/how-to-get-the-modulus-in-lisp
 *
 * Seemed easier to implement than mod.
 */
static fe_Object* cfunc_rem(fe_Context *ctx, fe_Object *arg)
{
    int x = (int)fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    int y = (int)fe_tonumber(ctx, fe_nextarg(ctx, &arg));
    return fe_number(ctx, x % y);
}


static fe_Object* cfunc_power_get(fe_Context *ctx, fe_Object *arg)
{
    char name[32];
    fe_tostring(ctx, fe_nextarg(ctx, &arg), name, sizeof name);

    if (strcmp(name, "valid") == 0) return fe_number(ctx, power_board_status.valid);
#define pbstateC(n, conv) \
    else if (strcmp(name, #n) == 0) return fe_number(ctx, power_board_status.n * conv);
#define pbstate(n) pbstateC(n, 1)
    pbstate(time)
    pbstate(mode)
    pbstate(duty)
    pbstate(RPM)
    pbstateC(voltage, 0.1)
    pbstateC(current, 0.001)
    pbstate(enabled)
    pbstateC(temperature_heatsink, 0.1)
    pbstate(fan)
    pbstate(error_count)
#undef pbstate
#undef pbstateC
    else
    {
        fe_error(ctx, "invalid power state name");
        return nullptr; // this should never happen, fe_error either exits or longjmps
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

        if (mode < 0 || mode > mode_max)
        {
            fe_error(ctx, "invalid mode");
            return nullptr;
        }
        power_board_set_mode((power_board_mode_t)mode);
    }
    // there should be no need to clear_errors
    else
    {
        fe_error(ctx, "invalid pwrs command");
        return nullptr; // this should never happen, fe_error either exits or longjmps
    }

    // TODO cannot find nil - seems to be private / static
    return fe_number(ctx, 1);
}


static int gc;
static fe_Context *ctx;

void lisp_init()
{
    ctx = fe_open(lisp_buf, sizeof(lisp_buf));
    fe_handlers(ctx)->error = onerror;

    gc = fe_savegc(ctx);

    fe_set(ctx, fe_symbol(ctx, "pwrg"), fe_cfunc(ctx, cfunc_power_get));
    fe_set(ctx, fe_symbol(ctx, "pwrs"), fe_cfunc(ctx, cfunc_power_set));
    fe_set(ctx, fe_symbol(ctx, "rem"), fe_cfunc(ctx, cfunc_rem));

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


static fe_Object * lisp_execute(lisp_str_t * lstr)
{
    bool jumped_in = false;
    setjmp(error_jmp);
    if (jumped_in) return nullptr;
    jumped_in = true;  //cppcheck-suppress unreadVariable

    fe_Object *obj;
    obj = fe_read(ctx, lisp_read_str, lstr);
    if (obj != nullptr)
    {
        obj = fe_eval(ctx, obj);
    }

    fe_restoregc(ctx, gc);
    return obj;
}


/**
 * Run lisp code and discard the output.
 * Errors will NOT be printed out.
 *
 * @param code Array of characters without null terminator
 * @param length Length of code array
 * @return true on success, false if error occurred.
 */
bool lisp_run_blind(const char * code, size_t length)
{
    error_occured = false;
    error_stream = nullptr;
    lisp_str_t lstr = { code, length, 0 };
    // don't care about the result, execute all root-level expressions
    while (lisp_execute(&lstr) != nullptr);
    return !error_occured;
}


/**
 * Run lisp code and discard the output.
 * Errors will NOT be printed out.
 *
 * @param code null-terminated string containing the code to run
 * @return true on success, false if error occurred.
 */
bool lisp_run_blind(const char * code)
{
    return lisp_run_blind(code, -1);
}


/**
 * Evaluate single lisp expression and print out the result to a Stream.
 *
 * @param code Array of characters without null terminator
 * @param length Length of code array
 * @param response Stream to print the result to
 */
bool lisp_process(const char * code, size_t length, Stream * response)
{
    error_stream = response;

    lisp_str_t lstr = { code, length, 0 };
    fe_Object * obj = lisp_execute(&lstr);
    if (obj != nullptr)
    {
        fe_write(ctx, obj, lisp_write_Stream, response, 0);
        return true;
    }
    return false;
}


/**
 * Evaluate single lisp expression and print out the result to a Stream.
 *
 * @param code null-terminated string containing the code to run
 * @param response Stream to print the result to
 */
bool lisp_process(const char * code, Stream * response)
{
    return lisp_process(code, -1, response);
}
