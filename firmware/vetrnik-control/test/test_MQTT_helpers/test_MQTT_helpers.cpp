#include <unity.h>
#include <stdio.h>
#include <stdbool.h>
#include <MQTT_helpers.h>

void test_COND_NEQ()
{
    int v1 = 1;
    int prev_v1 = 1;
    TEST_ASSERT_FALSE(COND_NEQ(v1));

    int v2 = 1;
    int prev_v2 = 2;
    TEST_ASSERT_TRUE(COND_NEQ(v2));

    bool v3 = 10;
    bool prev_v3 = 11;
    TEST_ASSERT_FALSE(COND_NEQ(v3));

    bool v4 = 0;
    bool prev_v4 = 11;
    TEST_ASSERT_TRUE(COND_NEQ(v4));
}


void test_COND_HYST()
{
    uint16_t v = 1;
    uint16_t prev_v = 1;
    TEST_ASSERT_FALSE(COND_HYST(v, 0));

    v = 2;
    TEST_ASSERT_TRUE(COND_HYST(v, 0));
    TEST_ASSERT_FALSE(COND_HYST(v, 1));
    v = 3;
    TEST_ASSERT_TRUE(COND_HYST(v, 0));
    TEST_ASSERT_TRUE(COND_HYST(v, 1));
    TEST_ASSERT_FALSE(COND_HYST(v, 2));

    prev_v = 5;
    TEST_ASSERT_TRUE(COND_HYST(v, 0));
    TEST_ASSERT_TRUE(COND_HYST(v, 1));
    TEST_ASSERT_FALSE(COND_HYST(v, 2));
}


void test_MAKETMP_BOOL()
{
    bool v1 = true;
    {
        MAKETMP_BOOL(v1);
        TEST_ASSERT_EQUAL_STRING("1", tmp);
    }

    v1 = false;
    {
        MAKETMP_BOOL(v1);
        TEST_ASSERT_EQUAL_STRING("0", tmp);
    }

    {
        MAKETMP_BOOL(false || true);
        TEST_ASSERT_EQUAL_STRING("1", tmp);
    }

    {
        MAKETMP_BOOL(false && true);
        TEST_ASSERT_EQUAL_STRING("0", tmp);
    }

}


void test_MAKETMP_UINT()
{
    uint16_t v1 = 41234;
    {
        MAKETMP_UINT(v1);
        TEST_ASSERT_EQUAL_STRING("41234", tmp);
    }

    v1 = (uint16_t)65536;
    {
        MAKETMP_UINT(v1);
        TEST_ASSERT_EQUAL_STRING("0", tmp);
    }

    uint32_t v2 = 65536;
    {
        MAKETMP_UINT(v2);
        TEST_ASSERT_EQUAL_STRING("65536", tmp);
    }

    v2 = 4294967295;
    {
        MAKETMP_UINT(v2);
        TEST_ASSERT_EQUAL_STRING("4294967295", tmp);
    }
}


void test_MAKETMP_DECIMAL()
{
    uint16_t v1 = 41234;
    {
        // 0 decimal places is valid input, but it shouldn't be used.
        // It also generates a compiler warning: repeated '0' flag in format
#pragma GCC diagnostic ignored "-Wformat"
        MAKETMP_DECIMAL(v1, 0);
#pragma GCC diagnostic pop
        TEST_ASSERT_EQUAL_STRING("41234.0", tmp);
    }
    {
        MAKETMP_DECIMAL(v1, 1);
        TEST_ASSERT_EQUAL_STRING("4123.4", tmp);
    }
    {
        MAKETMP_DECIMAL(v1, 4);
        TEST_ASSERT_EQUAL_STRING("4.1234", tmp);
    }
    {
        MAKETMP_DECIMAL(v1, 5);
        TEST_ASSERT_EQUAL_STRING("0.41234", tmp);
    }

    v1 = 2;
    {
        MAKETMP_DECIMAL(v1, 5);
        TEST_ASSERT_EQUAL_STRING("0.00002", tmp);
    }

    uint32_t v2 = 65536;
    {
        MAKETMP_DECIMAL(v2, 5);
        TEST_ASSERT_EQUAL_STRING("0.65536", tmp);
    }

    v2 = -2;
    {
        MAKETMP_DECIMAL(v2, 10);
        TEST_ASSERT_EQUAL_STRING("0.4294967294", tmp);
    }

    v2 = -1;
    {
        MAKETMP_DECIMAL(v2, 5);
        TEST_ASSERT_EQUAL_STRING("42949.67295", tmp);
    }
    {
        MAKETMP_DECIMAL(v2, 9);
        TEST_ASSERT_EQUAL_STRING("4.294967295", tmp);
    }
    {
        MAKETMP_DECIMAL(v2, 10);
        // Only for (uint32_t)(-1) and 10 decimal places,
        // MAKETMP_DECIMAL will fail.
        // This is because of the internal division and modulo operations done
        // in unsigned int.
        //TEST_ASSERT_EQUAL_STRING("0.4294967295", tmp);
        TEST_ASSERT_EQUAL_STRING("1.0000000000", tmp);
    }
}


int runUnityTests()
{
    UNITY_BEGIN();
    RUN_TEST(test_COND_NEQ);
    RUN_TEST(test_COND_HYST);
    RUN_TEST(test_MAKETMP_BOOL);
    RUN_TEST(test_MAKETMP_UINT);
    RUN_TEST(test_MAKETMP_DECIMAL);
    return UNITY_END();
}


int main()
{
    return runUnityTests();
}
