#include "control.h"
#include "settings.h"
#include "power_board.h"
#include "lisp.h"
#include "debug.h"
#include "mqtt.h"
#include "pump.h"
#include <Arduino.h>

static control_strategy_t strategy = control_shorted;


#define X_STR(name, value) #name,
/**
 * Array with strings corresponding to control_strategy_t values.
 * Terminated by nullptr.
 *
 * Useful for performing reverse lookup.
 */
const char * control_strategies[] = {
    CONTROL_STRATEGIES(X_STR)
    nullptr
};
#undef X_STR


void control_init()
{
    control_set_strategy(control_shorted);
}


/**
 * Must be called whenever lisp is (re)initialized.
 */
void control_init_lisp()
{
    lisp_empty_func("ctrl");
}


/**
 * To be called when a new state object is received from power board.
 * Also called when state is invalidated after timeout.
 */
void control_new_state()
{
    if (!power_board_status.valid && strategy != control_shorted)
    {
        // If status becomes invalid, it means that communication with power
        // board was lost. This function isn't called before the first status
        // message is received, so no problems at startup.
        control_set_strategy(control_shorted);
    }

    switch (strategy)
    {
        case control_shorted:
            if (power_board_status.mode != shorted)
            {
                INFO->println("control_shorted, but mode != shorted");
                power_board_set_mode(shorted);
            }
            break;

        case control_MQTT:
            if (millis() - MQTT_last_command_ms >= 30000UL)
            {
                INFO->println("MQTT control timeout");
                control_set_strategy(control_shorted);
            }
            break;

        case control_lisp:
            bool success = lisp_run_blind("(ctrl)");
            if (!success)
            {
                INFO->println("error in control_lisp");
                control_set_strategy(control_shorted);
            }
            break;
    }
}


void control_loop()
{
    // Handle external e-stop button, hardware OVP, ...
    static bool prev_short_emergency = false;
    static unsigned long short_emergency_millis = 0;
    bool short_emergency = (
            control_get_strategy() != control_shorted &&
            power_board_status.mode != shorted &&
            power_board_status.mode != stopping && // stopping mode can sometimes SHORT
            !digitalRead(PIN_SHORT_SENSE)
        );

    // the contactor is slow
    if (prev_short_emergency && millis() - short_emergency_millis >= 100UL)
    {
        INFO->println("SHORT e-stop");
        control_set_strategy(control_shorted);
        short_emergency = false;
    }

    if (short_emergency && !prev_short_emergency)
        short_emergency_millis = millis();
    prev_short_emergency = short_emergency;
}


void control_set_strategy(control_strategy_t s)
{
    INFO->print("Switching to ");
    INFO->println(control_strategies[s]);

    switch (s)
    {
        case control_shorted:
            power_board_set_duty(0);
            power_board_set_mode(shorted);
            power_board_set_software_enable(false);
            pump_set(false);
            digitalWrite(PIN_SHORT, LOW);
            break;

        case control_manual:
        case control_MQTT:
        case control_lisp:
            digitalWrite(PIN_SHORT, HIGH);
            break;
    }
    strategy = s;
}


bool control_set_strategy(const char * str)
{
    for (size_t i = 0; control_strategies[i] != nullptr; i++)
    {
        if (strcmp(str, control_strategies[i]) == 0)
        {
            control_set_strategy((control_strategy_t)i);
            return true;
        }
    }
    return false;
}


control_strategy_t control_get_strategy()
{
    return strategy;
}
