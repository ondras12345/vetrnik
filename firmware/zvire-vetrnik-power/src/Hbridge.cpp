#include "Hbridge.h"
#include "globals.h"

#include <avr/io.h>
#include <avr/interrupt.h>


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


static void start_timer()
{
    cli();
    TCCR1B =
        (1<<WGM13)  // mode 8: PWM, phase correct and frequency correct
        | (1<<CS11) | (1<<CS10)  // clkio / 64
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
    //pinMode(pin_PWM1, OUTPUT);
    //pinMode(pin_PWM2, OUTPUT);
    DDRB |= (1<<PB1) | (1<<PB2);  // OC1A and OC1B are outputs

    DDRD |= (1<<PD7);  // pin_SD
    Hbridge_set_enabled(false);

    start_timer();
}
