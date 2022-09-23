#include "Hbridge.h"
#include "globals.h"
#include "settings.h"
#include "hardware.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#define Hbridge_frequency 50  // Hz ; do NOT modify (even when switching prescaler to 400 Hz mode)

static constexpr uint16_t top_value = (F_CPU / (Hbridge_frequency * 2 * 64));
static constexpr uint32_t duty_conversion = top_value / 51; // (top_value / 255 / 2 * 10)


void Hbridge_set_duty(uint8_t duty_cycle)
{
    if (duty_cycle > Hbridge_duty_max) duty_cycle = Hbridge_duty_max;
    uint16_t a = (uint32_t)(duty_conversion) * duty_cycle / 10;
    uint16_t b = top_value - a;

    uint8_t oldSREG = SREG;
    cli();
    OCR1A = a;
    OCR1B = b;
    SREG = oldSREG;
}


/// If set to false, all transistors turn off.
void Hbridge_set_enabled(bool value)
{
    if (value)
        PORTD |= (1<<PD7);  // pin_SD
    else
        PORTD &= ~(1<<PD7);  // pin_SD
}


void Hbridge_init()
{
    // must be PB1 and PB2 (OC1A, OC1B)
    gpio_conf(pin_PWM1, OUTPUT, 0);
    gpio_conf(pin_PWM2, OUTPUT, 0);

    gpio_conf(pin_SD, OUTPUT, 0);
    Hbridge_set_enabled(false);

    // start timer
    cli();
    // 0b011 ... clkio / 64
    // 0b010 ... clkio / 8
    uint8_t CS_prescaler = settings[kHBridgeFrequency].value ? 0b010 : 0b011;
    TCCR1B =
        (1<<WGM13)  // mode 8: PWM, phase correct and frequency correct
        | ((CS_prescaler & 0x07) << CS10);
        ;

    // ICR1 is TOP value in mode 8
    ICR1 = top_value;

    set_duty(0);

    TCCR1A =
        // Clear OC1A on Compare Match when upcounting.
        // Set OC1A on Compare Match when downcounting.
        (1<<COM1A1)
        // Set OC1B on Compare Match when upcounting.
        // Clear OC1B on Compare Match when downcounting.
        | (1<<COM1B0) | (1<<COM1B1)
        ;

    sei();
}
