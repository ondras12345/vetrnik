#include <unity.h>

#include <stdio.h>
#include <stdint.h>
#include <moving_averages.h>


void test_MovingAverage_4()
{
    MovingAverage<4> avg;
    uint16_t samples[] = {
        23, 1020, 65500, 10,
        0, 0, 0, 0,
        65535, 65535, 65535, 65535,
        120
    };
    uint16_t results[sizeof samples / sizeof samples[0]] = {
        23, 521, 22181, 16638,
        16632, 16377, 2, 0,
        16383, 32767, 49151, 65535,
        49181
    };

    for (size_t i = 0; i < sizeof samples / sizeof samples[0]; i++)
    {
        TEST_ASSERT_EQUAL_UINT16(results[i], avg.process(samples[i]));
    }
}


void test_MovingAverage_2_reinit()
{
    MovingAverage<2> avg;
    uint16_t samples[] = {
        23,
        1020, 65500, 10,
        0, 0,
        // reinit at i==6
        65535,
        128, 120, 120
    };
    uint16_t results[sizeof samples / sizeof samples[0]] = {
        23,
        521, 33260, 32755,
        5, 0,
        // reinit
        65535,
        32831, 124, 120
    };

    for (size_t i = 0; i < sizeof samples / sizeof samples[0]; i++)
    {
        if (i == 6) avg.reinit();
        TEST_ASSERT_EQUAL_UINT16(results[i], avg.process(samples[i]));
    }
}


void test_MovingAverage_4_reinit()
{
    MovingAverage<4> avg;
    uint16_t samples[] = {
        23, 1020, 65500, 10, 0, 0,
        // reinit at i==6
        65535, 65535, 120, 120, 120, 120
    };
    uint16_t results[sizeof samples / sizeof samples[0]] = {
        23, 521, 22181, 16638, 16632, 16377,
        // reinit
        65535, 65535, 43730, 32827, 16473, 120
    };

    for (size_t i = 0; i < sizeof samples / sizeof samples[0]; i++)
    {
        if (i == 6) avg.reinit();
        TEST_ASSERT_EQUAL_UINT16(results[i], avg.process(samples[i]));
    }
}


void test_RMSFilter_basic()
{
    RMSFilter<10, 512> rms;
    TEST_ASSERT_EQUAL_UINT16(512, rms.process(512));
    TEST_ASSERT_LESS_THAN_UINT16(512, rms.process(0));
    for (int i = 0; i < 10; i++) rms.process(0);
    TEST_ASSERT_LESS_THAN_UINT16(300, rms.process(0));
}


int runUnityTests(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_MovingAverage_4);
    RUN_TEST(test_MovingAverage_2_reinit);
    RUN_TEST(test_MovingAverage_4_reinit);
    RUN_TEST(test_RMSFilter_basic);
    return UNITY_END();
}


int main(void)
{
    return runUnityTests();
}
