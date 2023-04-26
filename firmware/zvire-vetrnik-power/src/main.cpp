#include <Arduino.h>
#include <avr/wdt.h>

#include "hardware.h"
#include "uart.h"
#include "Hbridge.h"
#include "RPM.h"
#include "adc.h"
#include "globals.h"
#include "settings.h"

#include <errm.h>
const byte ERRM_COUNT_MAX=5;
errm_error errm_errors[ERRM_COUNT_MAX];
void error_create_callback(const errm_error *err);
#include "error_templates.h"


static unsigned long mode_prev_millis = 0;

bool set_mode(mode_t new_mode)
{
    if (emergency) return false;
    switch (new_mode)
    {
        case shorted:
            gpio_clr(pin_SHORT);
            Hbridge_set_enabled(false);
            mode = shorted;
            break;

        case stopping:
            {
                if (mode != const_duty) return false;
                // TODO switch all relays on
                Hbridge_set_duty(255);
                // Not setting duty to be able to recover after enable input
                // goes back up
                mode_prev_millis = millis();
            }
            mode = stopping;
            break;

        case const_duty:
            {
                if (mode != stopping) return false;
                // Stopping mode can sometimes SHORT if it detects it is not
                // effective by itself.
                gpio_set(pin_SHORT);
                Hbridge_set_duty(duty);
            }
            mode = const_duty;
            break;

        case start:
            {
                if (mode != shorted) return false;
                if (emergency) return false;
                if (!enabled) return false;
                Hbridge_set_duty(0);
                duty = 0;
                Hbridge_set_enabled(true);
                gpio_set(pin_SHORT);
            }
            mode = const_duty;
            break;
    }
    return true;
}


void error_create_callback(const errm_error *err)
{
    if (err->errortemplate->weight >= 10 && !emergency)
    {
        emergency_stop();
    }
}


// See avr/wdt.h
// https://www.avrfreaks.net/forum/init3-section-not-executed
uint8_t mcusr_mirror __attribute__ ((section (".noinit")));

void get_mcusr(void) __attribute__((naked, used, section(".init3")));
void get_mcusr(void)
{
    mcusr_mirror = MCUSR;
    MCUSR = 0;
    wdt_disable();
}


void setup()
{
    errm_create_callback = error_create_callback;

    gpio_conf(pin_FAN, OUTPUT, 1);
    gpio_conf(pin_SHORT, OUTPUT, 0);

    // We don't use Arduino's yield, so _delay_ms should be OK (less flash space)
    _delay_ms(10);  // Vcc should be stable before EEPROM is written to.

    uart_init();
    settings_init();

    gpio_conf(pin_REL1, OUTPUT, 0);
    gpio_conf(pin_REL2, OUTPUT, 0);
    gpio_conf(pin_REL3, OUTPUT, 0);
    gpio_conf(pin_REL4, OUTPUT, 0);

    //gpio_conf(pin_VSENSE, INPUT, NOPULLUP);
    //gpio_conf(pin_ACS712, INPUT, NOPULLUP);
    //gpio_conf(pin_ACS712_2, INPUT, NOPULLUP);
    //gpio_conf(pin_NTC1, INPUT, NOPULLUP);

    RPM_init();
    ADC_init();
    Hbridge_init();
    gpio_conf(pin_ENABLE, INPUT, PULLUP);

    if (mcusr_mirror & (1<<WDRF))
    {
        errm_add(errm_create(&etemplate_WDT_reset, mcusr_mirror));
    }

    _delay_ms(100);

    wdt_enable(WDTO_1S);
    wdt_reset();
    Serial.println(F(">boot"));
}


void loop()
{
    RPM_loop();
    ADC_loop();
    uart_loop();

    enabled = !gpio_rd8(PIN, pin_ENABLE);
    if (!enabled && mode != stopping && mode != shorted)
    {
        set_mode(stopping);
    }
    if (enabled && mode == stopping)
    {
        set_mode(const_duty);
    }

    switch (mode)
    {
        case stopping:
            if (millis() - mode_prev_millis >= stopping_time)
            {
                if (RPM >= stopping_RPM)
                {
                    gpio_clr(pin_SHORT);
                }
            }
            break;

        default:
            break;
    }

    wdt_reset();
}
