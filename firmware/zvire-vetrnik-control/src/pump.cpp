#include "pump.h"
#include "settings.h"
#include "log.h"
#include <Arduino.h>

void pump_set(bool state)
{
    if (state) log_add_event(kPumpOn);
    else log_add_event(kPumpOff);
    digitalWrite(PIN_REL1, state);
}


bool pump_get()
{
    return digitalRead(PIN_REL1);
}
