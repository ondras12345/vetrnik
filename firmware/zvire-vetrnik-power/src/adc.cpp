#include "adc.h"
#include "globals.h"
#include "settings.h"
#include "error_templates.h"
#include "hardware.h"
#include <moving_averages.h>
#include <NTC.h>
#include <Arduino.h>

// TODO https://majenko.co.uk/blog/our-blog-1/making-accurate-adc-readings-on-the-arduino-25


static volatile uint16_t ADC_values[4] = {0};
static volatile bool ADC_done = false;


void ADC_init()
{
    // Do NOT use analogRead!

    ADMUX = (1<<REFS0);   // Avcc with external capacitor at AREF pin
    ADCSRA = (1<<ADEN)    // enable ADC
            | (1<<ADIE)   // ADC interrupt enable
            | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0)  // prescaler 128
            ;
    // ATmega8 doesn't have ADCSRB, cannot trigger conversion with timer
    // compare match

    // Set up timer2 to start conversion every 1.024 ms
    // (It isn't easy to hijack timer0's interrupt)
    TCCR2 = (1<<CS22);  // prescaler 64
    // Enable interrupt on timer2 overflow
    TIMSK |= (1<<TOIE2);
}


ISR(TIMER2_OVF_vect)
{
    // Do not mess with loop() reading results. This should hopefully not
    // happen. TODO add a metric for debugging
    if (ADC_done) return;

    // If a conversion is already running, reset by watchdog.
    // This should never happen.
    if (ADCSRA & (1<<ADSC))
    {
        for (;;);
    }

    // Start new conversion
    // at channel 0 (set either by ADC_init or ISR(ADC_vect)
    ADCSRA |= (1<<ADSC);
}


//#define RMS_INITIAL 512  /* Initial value of the filter memory. */
//#define RMS_SAMPLES 512
//uint16_t rms_filter(uint16_t sample)
//{
//    static uint16_t rms = RMS_INITIAL;
//    static uint32_t sum_squares = 1UL * RMS_SAMPLES * RMS_INITIAL * RMS_INITIAL;
//
//    sum_squares -= sum_squares / RMS_SAMPLES;
//    sum_squares += (uint32_t) sample * sample;
//    if (rms == 0) rms = 1;    /* do not divide by zero */
//    rms = (rms + sum_squares / RMS_SAMPLES / rms) / 2;
//    return rms;
//}

ISR(ADC_vect)
{
    uint8_t channel = ADMUX & 0x0f;
    if (channel > 3)
    {
        for (;;);  // this should never happen, reset by watchdog
        return;  // make cppcheck happy
    }

    // get value
    uint8_t low = ADCL;
    uint8_t high = ADCH;
    // Advance to next channel
    ADMUX = (ADMUX & 0xf0) | ((channel + 1) & 0x03);
    if (channel == 3)
    {
        ADC_done = true;
    }
    else
    {
        // Start new conversion
        ADCSRA |= (1<<ADSC);
    }

    ADC_values[channel] = (high << 8) | low;
}


uint16_t map_uint16(uint16_t x,
                    uint16_t in_min, uint16_t in_max,
                    uint16_t out_min, uint16_t out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void ADC_loop()
{


    if (ADC_done)
    {
        // ADC_values cannot change if ADC_done is true (see ISRs)
        static RMSFilter<> RMS_voltage;
        uint16_t voltage_mV = RMS_voltage.process(ADC_values[0]) * 320000UL >> 16;  // same as *5000 / 1024
        voltage = voltage_mV * (voltage_R1 + voltage_R2) / voltage_R2 / 100UL;  // voltage is *10 fixed-point
        // TODO OVP on peak voltage
        // TODO current
        current = ADC_values[1];
        MovingAverage<4> avg_NTC;
        ADC_values[3] = avg_NTC.process(ADC_values[3]);

        ADC_done = false;  // unlock the interrupt as soon as possible

        static unsigned long NTC_prev_millis = 0;
        if (millis() - NTC_prev_millis >= 500UL)
        {
            NTC_prev_millis = millis();
            NTC ntc_heatsink(NTC_heatsink_Rdiv, NTC_heatsink_beta, NTC_heatsink_R_nom);
            temperature_heatsink = ntc_heatsink.getC(ADC_values[3]);
            if (temperature_heatsink == 0 || temperature_heatsink > heatsink_temperature_max)
            {
                errm_add(errm_create(&etemplate_temperature, (temperature_heatsink & 0xFF)));
            }
            //if (temperature_heatsink < fan_temperature_off) fan = fan_power_min;
            //else if (temperature_heatsink > fan_temperature_full) fan = 255;
            //fan = map_uint16(temperature_heatsink, fan_temperature_off, fan_temperature_full, fan_power_min, 255);
            fan = (temperature_heatsink > (fan_temperature_off + (fan ? 0 : 1))) ? 255 : 0;
            // TODO PD5 cannot do PWM on ATmega8...
            if (fan < 128)
                gpio_clr(pin_FAN);
            else
                gpio_set(pin_FAN);
        }

        if (voltage <= voltage_protection_start && OVP_stop)
        {
            OVP_stop = false;
            // Restore mode from before OVP kicked in
            set_mode(OVP_mode);
        }

        if (voltage >= voltage_protection_stop && !OVP_stop && !emergency)
        {
            OVP_stop = true;
            OVP_mode = mode;
            set_mode(stopping);
            errm_add(errm_create(&etemplate_OVP_stop));
        }

        if (voltage >= voltage_protection_short && !emergency)
        {
            // Error with weight 10, call emergency_stop automatically
            errm_add(errm_create(&etemplate_OVP_short));
        }
    }
}
