#pragma once

#include <stdint.h>
#include <stdbool.h>


/**
 * A moving average for ADC samples (or other uint16_t).
 * Only suitable for low values of N, because it uses an actual buffer to store all samples in memory.
 */
template <uint8_t N>
class MovingAverage
{
    public:
        uint16_t process(uint16_t sample);
        void reinit();

    private:
        uint8_t inited = 0;
        uint32_t sum = 0;
        uint8_t pos = 0;
        uint16_t buff[N] = { 0 };
};


/**
 * Reinitialize the moving average.
 */
template <uint8_t N>
void MovingAverage<N>::reinit()
{
    inited = 0;
    sum = 0;
}


template <uint8_t N>
uint16_t MovingAverage<N>::process(uint16_t sample)
{
    pos++;
    if (pos >= N) pos = 0;

    if (inited < N)
    {
        inited++;
        buff[pos] = sample;
        sum += sample;
    }
    else
    {
        sum = sum - buff[pos] + sample;
        buff[pos] = sample;
    }

    return sum / inited;
}



// https://stackoverflow.com/questions/28807537/any-faster-rms-value-calculation-in-c
template <uint16_t N = 512, uint16_t INIT = 512>
class RMSFilter
{
    public:
        uint16_t process(uint16_t sample);
    private:
        uint16_t rms = INIT;
        uint32_t sum_squares = 1UL * N * INIT * INIT;
};


template <uint16_t N, uint16_t INIT>
uint16_t RMSFilter<N, INIT>::process(uint16_t sample)
{
    sum_squares -= sum_squares / N;
    sum_squares += (uint32_t) sample * sample;
    if (rms == 0) rms = 1;    /* do not divide by zero */
    rms = (rms + sum_squares / N / rms) / 2;
    return rms;
}
