#include "pump.h"
#include "settings.h"
#include <Arduino.h>

void pump_set(bool state)
{
    digitalWrite(PIN_REL1, state);
}


bool pump_get()
{
    return digitalRead(PIN_REL1);
}
