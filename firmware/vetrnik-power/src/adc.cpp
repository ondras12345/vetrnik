#include "adc.h"
#include "globals.h"
#include "settings.h"
#include "error_templates.h"
#include "hardware.h"
#include "Hbridge.h"
#include <moving_averages.h>
#include <NTC.h>
#include <Arduino.h>
#include <util/atomic.h>

// TODO https://majenko.co.uk/blog/our-blog-1/making-accurate-adc-readings-on-the-arduino-25


static volatile uint16_t ADC_values[4] = {0};


void ADC_init()
{
    // Do NOT use analogRead!

    ADMUX = (1<<REFS0);   // Avcc with external capacitor at AREF pin
    ADCSRA = (1<<ADEN)    // enable ADC
            | (1<<ADIE)   // ADC interrupt enable
            | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0)  // prescaler 128
            ;


    // For 400Hz switching frequency, we need faster voltage / current sampling
    ADCSRA |= (1<<ADSC);  // start conversion
    return;
}


ISR(ADC_vect)
{
    uint8_t channel = ADMUX & 0x0F;
    if (channel > 3)
    {
        for (;;);  // this should never happen, reset by watchdog
        return;  // make cppcheck happy
    }

    // get value
    // ADCL must be read first.
    uint8_t low = ADCL;
    uint8_t high = ADCH;
    ADC_values[channel] = (high << 8) | low;

    // TODO fix start value of RMS filters
    // TODO tweak filter constants (how many samples per second?)
    static RMSFilter<> RMS_voltage;
    static RMSFilter<> RMS_current;

    if (channel == 0) ADC_values[0] = RMS_voltage.process(ADC_values[0]);
    if (channel == 1) ADC_values[1] = RMS_current.process(ADC_values[1]);

    uint8_t next_channel = (channel + 1) % 4;
    // only sample NTCs once in a while
    static uint8_t NTC_counter = 0;
    if (next_channel == 2)
    {
        if (NTC_counter != 0) next_channel = 3;
    }
    if (next_channel == 3)
    {
        if (NTC_counter != 0) next_channel = 0;
        NTC_counter++;
    }

    // Advance to next channel
    ADMUX = (ADMUX & 0xF0) | next_channel;


    // Start new conversion
    // This works out to one conversion per 104 us
    ADCSRA |= (1<<ADSC);
}


uint16_t map_uint16(uint16_t x,
                    uint16_t in_min, uint16_t in_max,
                    uint16_t out_min, uint16_t out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


static void OVP() {
    static mode_t OVP_mode;
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
        // Error with weight 10 will call emergency_stop automatically
        errm_add(errm_create(&etemplate_OVP_short));
    }
}


void ADC_loop()
{
    // 100 ms is a short time interval, so unsigned int is sufficient.
    const unsigned int now = millis();
    static unsigned int prev_ms = 0;
    if (now - prev_ms >= 100U)
    {
        prev_ms = now;
        uint16_t vals[4];
        ATOMIC_BLOCK(ATOMIC_FORCEON)
        {
            vals[0] = ADC_values[0];
            vals[1] = ADC_values[1];
            vals[2] = ADC_values[2];
            vals[3] = ADC_values[3];
        }

        uint16_t voltage_mV = vals[0] * 320000UL >> 16;  // same as *5000 / 1024
        voltage = voltage_mV * (voltage_R1 + voltage_R2) / voltage_R2 / 100UL;  // voltage is *10 fixed-point

        const uint16_t current_offset =
            settings[kCurrentOffsetL].value |
            settings[kCurrentOffsetH].value << 8;
        const uint8_t current_conversion = settings[KCurrentConversion].value;

        current = (vals[1] > current_offset) ? vals[1] - current_offset : 0;
        const uint16_t max_current = (current_conversion == 0) ? 0 : 65535U / current_conversion;
        if (current >= max_current)
            current = 65535U;
        else
            current = current * current_conversion;

        OVP();
        Hbridge_OCP();

        // only do this every 800 ms
        static uint8_t NTC_count = 7;  // update immediately after boot
        NTC_count = (NTC_count + 1) & 0x07;
        if (NTC_count == 0)
        {
            NTC ntc_heatsink(NTC_heatsink_Rdiv, NTC_heatsink_beta, NTC_heatsink_R_nom);
            NTC ntc_rectifier(NTC_rectifier_Rdiv, NTC_rectifier_beta, NTC_rectifier_R_nom);
            temperature_heatsink = ntc_heatsink.getC(vals[3]);
            temperature_rectifier = ntc_rectifier.getC(vals[2]);
            if (temperature_heatsink == 0 || temperature_heatsink > HEATSINK_TEMPERATURE_MAX)
            {
                errm_add(errm_create(&etemplate_temperature, ((temperature_heatsink/10) & 0xFF)));
            }
            if (temperature_rectifier == 0 || temperature_rectifier > RECTIFIER_TEMPERATURE_MAX)
            {
                errm_add(errm_create(&etemplate_temperature, ((temperature_rectifier/10) & 0xFF)));
            }
            uint8_t fan_hs;
            if (temperature_heatsink < FAN_TEMPERATURE_OFF) fan_hs = 0;
            else if (temperature_heatsink > FAN_TEMPERATURE_FULL) fan_hs = 255;
            else fan_hs = map_uint16(temperature_heatsink, FAN_TEMPERATURE_OFF, FAN_TEMPERATURE_FULL, 0, 255);
            uint8_t fan_rect = (temperature_rectifier > (FAN_TEMPERATURE_RECTIFIER_THRESHOLD + (fan ? -5 : 5))) ? 255 : 0;
            // pick max
            fan = (fan_hs > fan_rect) ? fan_hs : fan_rect;
            fan = (fan_manual > fan) ? fan_manual : fan;
            OCR2 = fan; // set fan PWM duty
        }
    }
}
