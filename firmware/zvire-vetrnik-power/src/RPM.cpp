#include "RPM.h"
#include "globals.h"
#include "hardware.h"
#include "settings.h"
#include <moving_averages.h>
#include <Arduino.h>

static volatile uint32_t period = 0;
static volatile unsigned long last_time = 0;
static volatile bool RPM_new = false;


static void RPM_ISR()
{
    unsigned long now = micros();
    period = now - last_time;
    last_time = now;
    RPM_new = true;
}


void RPM_init()
{
    pinMode(pin_RPM_1, INPUT);
    pinMode(pin_RPM_2, INPUT);

    attachInterrupt(digitalPinToInterrupt(pin_RPM_1), RPM_ISR, FALLING);
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
