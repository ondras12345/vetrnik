#include <Arduino.h>
#include "settings.h"

#ifdef watchdog_time
#include <IWatchdog.h>
#endif

#include "uart_power.h"
#include "mqtt.h"
#include "cli.h"
#include "debug.h"

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
#ifdef watchdog_time
    // Only do this after MQTT_init (DHCP is slow)
    IWatchdog.begin(watchdog_time*1000UL);
#endif
    uart_power_init();
    INFO.println("boot");
    CLI_init();
}


void loop()
{
    uart_power_loop();
    CLI_loop();
    MQTT_loop();
#ifdef watchdog_time
    IWatchdog.reload();
#endif
}
