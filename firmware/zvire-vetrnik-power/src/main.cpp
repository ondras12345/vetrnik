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
            digitalWrite(pin_SHORT, LOW);
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
                digitalWrite(pin_SHORT, HIGH);
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
                digitalWrite(pin_SHORT, HIGH);
            }
            mode = const_duty;
            break;

        // TODO
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

    pinMode(pin_FAN, OUTPUT);
    digitalWrite(pin_FAN, HIGH);
    pinMode(pin_SHORT, OUTPUT);
    digitalWrite(pin_SHORT, LOW);

    uart_init();

    pinMode(pin_REL1, OUTPUT);
    pinMode(pin_REL2, OUTPUT);
    pinMode(pin_REL3, OUTPUT);
    pinMode(pin_REL4, OUTPUT);

    //pinMode(pin_VSENSE, INPUT);
    //pinMode(pin_ACS712, INPUT);
    //pinMode(pin_ACS712_2, INPUT);
    //pinMode(pin_NTC1, INPUT);

    RPM_init();
    ADC_init();
    Hbridge_init();

    if (mcusr_mirror & (1<<WDRF))
    {
        // reset by watchdog
        for (;;)
        {
            Serial.print(F("Reset by watchdog "));
            Serial.println(mcusr_mirror);
            delay(500);
        }
    }

    pinMode(pin_ENABLE, INPUT_PULLUP);
    delay(100);

    wdt_enable(WDTO_1S);
    wdt_reset();
    Serial.println(F("boot"));
}


void loop()
{
    RPM_loop();
    ADC_loop();
    uart_loop();

    enabled = !digitalRead(pin_ENABLE);
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
                    digitalWrite(pin_SHORT, LOW);
                }
            }
            break;
    }

    wdt_reset();
}
