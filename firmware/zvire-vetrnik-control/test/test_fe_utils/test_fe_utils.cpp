#include <unity.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <fe_utils.h>


static uint8_t fe_buf[8*1024];
static int gc;
static fe_Context* ctx;
static bool error_expected = false;


void onerror(fe_Context *ctx, const char *msg, fe_Object *cl)
{
    char buff[128];
    if (error_expected) TEST_PASS();

    snprintf(buff, sizeof buff, "fe error: %s", msg);
    TEST_FAIL_MESSAGE(buff);
}


void setUp()
{
    error_expected = false;
    ctx = fe_open(fe_buf, sizeof fe_buf);
    fe_handlers(ctx)->error = onerror;
    gc = fe_savegc(ctx);
    fe_set(ctx, fe_symbol(ctx, "rem"), fe_cfunc(ctx, cfunc_rem));
    fe_set(ctx, fe_symbol(ctx, "round"), fe_cfunc(ctx, cfunc_round));
    fe_set(ctx, fe_symbol(ctx, "map"), fe_cfunc(ctx, cfunc_map));
    fe_set(ctx, fe_symbol(ctx, "empty"), fe_cfunc(ctx, cfunc_empty));
    fe_restoregc(ctx, gc);
}


void tearDown()
{
    fe_close(ctx);
}


fe_Object * run_str_code(const char* code)
{
    fe_str_t fstr = { code, (size_t)-1, 0 };
    fe_Object *obj = fe_read(ctx, fe_read_str, (void *) &fstr);
    TEST_ASSERT_NOT_NULL(obj);
    obj = fe_eval(ctx, obj);
    TEST_ASSERT_NOT_NULL(obj);
    fe_restoregc(ctx, gc);
    return obj;
}


void test_read_str()
{
    fe_Object *obj = run_str_code("(+ 1 (* 2 3 4))");
    TEST_ASSERT_EQUAL_FLOAT(25.0, fe_tonumber(ctx, obj));
}


void test_error()
{
    // missing )
    error_expected = true;
    fe_Object *obj = run_str_code("(+ 1 (* 2 3 4)");
    TEST_FAIL_MESSAGE("A fe error was expected");
}


void test_rem()
{
    fe_Object *obj;
    obj = run_str_code("(rem 2 2)");
    TEST_ASSERT_EQUAL_FLOAT(0.0, fe_tonumber(ctx, obj));

    obj = run_str_code("(rem 3 2)");
    TEST_ASSERT_EQUAL_FLOAT(1.0, fe_tonumber(ctx, obj));

    obj = run_str_code("(rem -1 5)");
    TEST_ASSERT_EQUAL_FLOAT(-1.0, fe_tonumber(ctx, obj));

    obj = run_str_code("(rem 13 4)");
    TEST_ASSERT_EQUAL_FLOAT(1.0, fe_tonumber(ctx, obj));
}


void test_rem_0()
{
    error_expected = true;
    fe_Object *obj = run_str_code("(rem 3 0)");
    TEST_FAIL_MESSAGE("A fe error was expected");
}


void test_map()
{
    fe_Object *obj;
    obj = run_str_code("(map 127.5 0 255 0 1)");
    TEST_ASSERT_EQUAL_FLOAT(0.5, fe_tonumber(ctx, obj));
    obj = run_str_code("(map 0.5 0 1 0 255)");
    TEST_ASSERT_EQUAL_FLOAT(127.5, fe_tonumber(ctx, obj));
    // TODO more test cases for map
}


void test_round()
{
    fe_Object *obj;
    // should round to even
    obj = run_str_code("(round 0.1)");
    TEST_ASSERT_EQUAL_FLOAT(0.0, fe_tonumber(ctx, obj));
    obj = run_str_code("(round 0.5)");
    TEST_ASSERT_EQUAL_FLOAT(0.0, fe_tonumber(ctx, obj));
    obj = run_str_code("(round 0.51)");
    TEST_ASSERT_EQUAL_FLOAT(1.0, fe_tonumber(ctx, obj));
    obj = run_str_code("(round 1.49)");
    TEST_ASSERT_EQUAL_FLOAT(1.0, fe_tonumber(ctx, obj));
    obj = run_str_code("(round 1.5)");
    TEST_ASSERT_EQUAL_FLOAT(2.0, fe_tonumber(ctx, obj));
    obj = run_str_code("(round -1.5)");
    TEST_ASSERT_EQUAL_FLOAT(-2.0, fe_tonumber(ctx, obj));
    obj = run_str_code("(round -0.5)");
    TEST_ASSERT_EQUAL_FLOAT(0.0, fe_tonumber(ctx, obj));
}


void test_stress()
{
    // Try to break things.
    fe_Object *obj;
    run_str_code("(= x 12)");
    for (unsigned int i = 0; i < 4000U; i++)
    {
        char varname = 'a' + (i % 10);
        char code[64];
        snprintf(code, sizeof code, "(= %c (+ 1 %d))", varname, i);
        run_str_code(code);
        code[0] = varname;
        code[1] = '\0';
        obj = run_str_code(code);
        TEST_ASSERT_EQUAL_FLOAT(1.0+i, fe_tonumber(ctx, obj));
    }

    obj = run_str_code("x");
    TEST_ASSERT_EQUAL_FLOAT(12.0, fe_tonumber(ctx, obj));

    // run another test to see if gc did not delete something important
    test_rem();
}


int runUnityTests()
{
    UNITY_BEGIN();
    RUN_TEST(test_read_str);
    RUN_TEST(test_error);
    RUN_TEST(test_rem);
    RUN_TEST(test_rem_0);
    RUN_TEST(test_map);
    RUN_TEST(test_round);
    RUN_TEST(test_stress);
    return UNITY_END();
}


int main()
{
    return runUnityTests();
}
