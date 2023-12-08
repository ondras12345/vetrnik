#include "RPM.h"
#include "globals.h"
#include "hardware.h"
#include "settings.h"
#include <moving_averages.h>
#include <Arduino.h>
#include <util/atomic.h>

static volatile uint32_t period = 0;
static volatile unsigned long last_time = 0;
static volatile bool RPM_new = false;
// This is too short
//#define SAMPLING_FREQUENCY 7812UL  // Hz
//#define DEBOUNCE_TIME 0.5  // ms  TODO try 4 ms
//#define INTEGRATOR_MAX (DEBOUNCE_TIME * SAMPLING_FREQUENCY / 1000)  // 3 is too low...
static volatile uint8_t integrator_max; // probably doesn't need to be volatile


ISR(TIMER2_OVF_vect)
{
    static uint8_t integrator = 0;

    if (gpio_rd8(PIN, pin_RPM_1))
    {
        if (integrator < integrator_max) integrator++;
    }
    else
    {
        if (integrator > 0) integrator--;
    }

    static bool output = false;
    static bool prev_output = false;
    if (integrator == 0)
    {
        output = false;
    }

    if (integrator >= integrator_max)
    {
        output = true;
        // this should never be needed
        integrator = integrator_max;
    }

    if (output != prev_output)
    {
        prev_output = output;
        if (!output)  // FALLING
        {
            unsigned long now = micros();
            if (RPM_new)
            {
                last_time = now;
                return;
            }
            period = now - last_time;
            last_time = now;
            RPM_new = true;
        }
    }
}


void RPM_init()
{
    gpio_conf(pin_RPM_1, INPUT, NOPULLUP);
    gpio_conf(pin_RPM_2, INPUT, NOPULLUP);

    integrator_max = settings[kRPMfilter].value;

    TCCR2 = (1<<CS21);  // prescaler 8, every 0.128 ms
    // Enable interrupt on timer2 overflow
    TIMSK |= (1<<TOIE2);
}


void RPM_loop()
{
    static MovingAverage<4> avg;
    static unsigned long lt;
    if (RPM_new)
    {
        uint32_t t;
        ATOMIC_BLOCK(ATOMIC_FORCEON)
        {
            t = period;
            lt = last_time;
            RPM_new = false;
        }
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
