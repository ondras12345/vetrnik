#include <Arduino.h>
#include "settings.h"

#ifdef WATCHDOG_TIME
#include <IWatchdog.h>
#endif

#include "uart_power.h"
#include "power_datapoints.h"
#include "mqtt.h"
#include "cli.h"
#include "debug.h"
#include "power_board.h"
#include "lisp.h"
#include "control.h"
#include "stats.h"
#include "display.h"
#include <SerialFlash.h>
#include <Bounce2.h>


static Bounce2::Button button1 = Bounce2::Button();
static Bounce2::Button button2 = Bounce2::Button();


void setup()
{
    Serial.begin();
    log_add_INFO_backend(&Serial);

    button1.attach(PIN_BUTTON1, INPUT_PULLUP);
    button1.interval(BUTTON_DEBOUNCE);
    button1.setPressedState(LOW);
    button2.attach(PIN_BUTTON2, INPUT_PULLUP);
    button2.interval(BUTTON_DEBOUNCE);
    button2.setPressedState(LOW);

    pinMode(PIN_BUZZER, OUTPUT);
    pinMode(PIN_SHORT, OUTPUT);
    pinMode(PIN_LED, OUTPUT);
    // PIN_ONEWIRE is unused
    pinMode(PIN_REL1, OUTPUT);
    pinMode(PIN_REL2, OUTPUT);

    digitalWrite(PIN_BUZZER, HIGH);
    pinMode(LED_BUILTIN, OUTPUT);
    for (uint8_t i = 0; i < 10; i++)
    {
        digitalWrite(LED_BUILTIN, LOW);
        delay(100);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
    }
    INFO->print("Flash init: ");
    INFO->println(SerialFlash.begin(PIN_FLASH_SS));
    INFO->println("settings_init");
    INFO->flush();
    settings_init();
    INFO->flush();
    MQTT_init();
#ifdef WATCHDOG_TIME
    // Only do this after MQTT_init (DHCP is slow)
    IWatchdog.begin(WATCHDOG_TIME*1000UL);
#endif
    uart_power_init();
    control_init();
    lisp_init();
    control_init_lisp();
    INFO->println("boot");
    CLI_init();
    display_init();
    display_init_lisp();
    lisp_run_blind_file(LISP_INIT_FILENAME);

    digitalWrite(PIN_BUZZER, LOW);
}


void loop()
{
    button1.update();
    if (button1.released())
    {
        if (button1.previousDuration() < BUTTON_LONG)
        {
            lisp_run_blind("(btn1_short)");
        }
        else if (button1.previousDuration() >= BUTTON_LONG)
        {
            lisp_run_blind("(btn1_long)");
        }
    }
    button2.update();
    if (button2.released())
    {
        if (button2.previousDuration() < BUTTON_LONG)
        {
            lisp_run_blind("(btn2_short)");
        }
        else if (button2.previousDuration() >= BUTTON_LONG)
        {
            lisp_run_blind("(btn2_long)");
        }
    }

    power_datapoints_loop();
    bool status_complete = uart_power_loop();
    if (status_complete)
    {
         power_board_status = power_board_status_read();
         control_new_state();
         stats_new_state();
    }
    else if (power_board_status.valid &&
             millis() - power_board_status.retrieved_millis >= 2000UL)
    {
        INFO->println("power_board_status timeout");
        power_board_status.valid = false;
        control_new_state();
        stats_new_state();
    }

    CLI_loop();
    MQTT_loop();
    display_loop();
#ifdef WATCHDOG_TIME
    IWatchdog.reload();
#endif
}
