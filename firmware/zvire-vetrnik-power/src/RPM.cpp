#include "RPM.h"
#include "globals.h"
#include "hardware.h"
#include "settings.h"
#include <moving_averages.h>
#include <Arduino.h>

static volatile uint32_t period = 0;
static volatile unsigned long last_time = 0;
static volatile bool RPM_new = false;


ISR(INT0_vect)
{
    unsigned long now = micros();
    period = now - last_time;
    //if (period < 500) return;  // low pass filter (2 kHz), doesn't work too well
    last_time = now;
    RPM_new = true;
}


void RPM_init()
{
    gpio_conf(pin_RPM_1, INPUT, NOPULLUP);
    gpio_conf(pin_RPM_2, INPUT, NOPULLUP);

    //attachInterrupt(digitalPinToInterrupt(pin_RPM_1), RPM_ISR, FALLING);
    MCUCR = (MCUCR & ~((1 << ISC00) | (1 << ISC01))) | (FALLING << ISC00);
    GICR |= (1 << INT0);
}


void RPM_loop()
{
    static MovingAverage<4> avg;
    static unsigned long lt;
    if (RPM_new)
    {
        RPM_new = false;
        uint32_t t;
        cli();
        t = period;
        lt = last_time;
        sei();
        if (t == 0)
        {
            RPM = 0;
            avg.reinit();
        }
        else
        {
            RPM = 1000000UL * settings[kRPMConversion].value / t;
            RPM = avg.process(RPM);
        }
    }
    else if (RPM != 0 && micros() - lt >= 1000000UL)
    {
        RPM = 0;
        avg.reinit();
    }
}
