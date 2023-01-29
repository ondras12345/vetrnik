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
#include <SerialFlash.h>


void setup()
{
    Serial.begin();
    log_add_INFO_backend(&Serial);
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
}


void loop()
{
    power_datapoints_loop();
    bool status_complete = uart_power_loop();
    if (status_complete)
    {
         power_board_status = power_board_status_read();
         control_new_state();
    }
    else if (power_board_status.valid &&
             millis() - power_board_status.retrieved_millis >= 2000UL)
    {
        INFO->println("power_board_status timeout");
        power_board_status.valid = false;
        control_new_state();
    }

    CLI_loop();
    MQTT_loop();
#ifdef WATCHDOG_TIME
    IWatchdog.reload();
#endif
}
