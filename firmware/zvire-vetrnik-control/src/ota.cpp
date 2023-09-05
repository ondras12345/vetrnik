#include "ota.h"
// Ethernet must be included before ArduinoOTA
#include <Ethernet.h>
#include <ArduinoOTA.h>
#include "settings.h"
#include "debug.h"
#include "log.h"


static bool OTA_enabled = false;

/**
 * Only call this if you want to enable OTA.
 * It is OK to call ota_loop without calling ota_init first.
 */
void ota_init()
{
    ArduinoOTA.begin(
        Ethernet.localIP(),
        settings.OTAname,
        settings.OTApassword,
        InternalStorage
    );
    OTA_enabled = true;
    log_add_event(kOtaEnabled);
}


void ota_loop()
{
    if (!OTA_enabled) return;
    static unsigned long prev = 0;
    unsigned long now = millis();
    if (now - prev >= 1000UL)
    {
        INFO->println("W: OTA is enabled");  // warning
        prev = now;
        digitalWrite(PIN_BUZZER, !digitalRead(PIN_BUZZER));
    }

    ArduinoOTA.poll();

}
