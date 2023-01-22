#include <Arduino.h>
#include "settings.h"

#ifdef WATCHDOG_TIME
#include <IWatchdog.h>
#endif

#include "uart_power.h"
#include "mqtt.h"
#include "cli.h"
#include "debug.h"
#include "power_board.h"
#ifdef LISP_REPL
#include "lisp.h"
#endif

void setup()
{
    Serial.begin();
    pinMode(LED_BUILTIN, OUTPUT);
    for (uint8_t i = 0; i < 10; i++)
    {
        digitalWrite(LED_BUILTIN, LOW);
        delay(100);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
    }
    DEBUG.println("settings_init");
    DEBUG.flush();
    settings_init();
    DEBUG.flush();
    MQTT_init();
#ifdef WATCHDOG_TIME
    // Only do this after MQTT_init (DHCP is slow)
    IWatchdog.begin(WATCHDOG_TIME*1000UL);
#endif
    uart_power_init();
#ifdef LISP_REPL
    lisp_init();
#endif
    INFO.println("boot");
    CLI_init();
}


void loop()
{
    bool status_complete = uart_power_loop();
    if (status_complete)
    {
         power_board_status = power_board_status_read();
    }

    CLI_loop();
    MQTT_loop();
#ifdef WATCHDOG_TIME
    IWatchdog.reload();
#endif
}
