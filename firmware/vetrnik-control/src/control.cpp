#include "control.h"
#include "settings.h"
#include "power_board.h"
#include "lisp.h"
#include "debug.h"
#include "mqtt.h"
#include "pump.h"
#include "log.h"
#include <Arduino.h>

static control_strategy_t strategy = control_shorted;
static unsigned long contactor_prev_millis = 0;
static bool contactor_state = false;


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
                log_add_event_and_println(kControlNotShorted, INFO);
                power_board_set_mode(shorted);
            }
            break;

        case control_MQTT:
            if (millis() - MQTT_last_command_ms >= 30000UL)
            {
                log_add_event_and_println(kControlMqttTimeout, INFO);
                control_set_strategy(control_shorted);
            }
            break;

        case control_lisp:
            {
                bool success = lisp_run_blind("(ctrl)");
                if (!success)
                {
                    log_add_event_and_println(kControlLispError, INFO);
                    control_set_strategy(control_shorted);
                }
            }
            break;

        case control_manual:
            break;
    }
}


void control_loop()
{
    const unsigned long now = millis();

    // control_contactor_set expiration
    if (now - contactor_prev_millis >= settings.contactor_debounce_min * 60000UL)
    {
        contactor_state = false;
    }

    // set contactor state
    digitalWrite(PIN_SHORT, contactor_state && strategy != control_shorted);

    // Handle external e-stop button, hardware OVP, ...
    static bool prev_short_emergency = false;
    static unsigned long short_emergency_millis = 0;
    // this value will be true if something is wrong (contactor is
    // shorting the generator but it is not expected to.)
    bool short_emergency = (
            control_get_strategy() != control_shorted &&
            power_board_status.mode != shorted &&
            power_board_status.mode != stopping && // stopping mode can sometimes SHORT
            contactor_state &&
            !digitalRead(PIN_SHORT_SENSE)
        );

    // the contactor is slow
    if (prev_short_emergency && now - short_emergency_millis >= 100UL)
    {
        log_add_event_and_println(kControlShortEstop, INFO);
        control_set_strategy(control_shorted);
        short_emergency = false;
    }

    if (short_emergency && !prev_short_emergency)
        short_emergency_millis = now;
    prev_short_emergency = short_emergency;
}


void control_set_strategy(control_strategy_t s)
{
    log_add_record_control_strategy(strategy, s);
    INFO->print("Switching to ");
    INFO->println(control_strategies[s]);

    switch (s)
    {
        case control_shorted:
            power_board_set_duty(0);
            power_board_set_mode(shorted);
            power_board_set_software_enable(false);
            pump_set(false);
            contactor_state = false;
            digitalWrite(PIN_SHORT, LOW);
            break;

        case control_manual:
        case control_MQTT:
        case control_lisp:
            // PIN_SHORT should be controlled via control_contactor_set()
            //digitalWrite(PIN_SHORT, HIGH);
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


/**
 * Control contactor state
 *
 * Call when you want to connect the generator to the device.
 * If all other conditions are met (e.g. !emergency), the contactor
 * will operate and be kept that way until settings.contactor_debounce_min
 * minutes pass since the last call to this function.
 *
 * In normal operation, this function should be called repeatedly as
 * long as the turbine is spinning.
 */
void control_contactor_set()
{
    contactor_prev_millis = millis();
    contactor_state = true;
}


/**
 * Get state of contactor control
 *
 * This checks whether the contactor is told to be on, not whether it is
 * actually energized. It could still be off e.g. if this function says on but
 * we are in emergency mode.
 *
 * @return remaining time in ms if on, -1 otherwise
 */
unsigned long control_contactor_get()
{
    if (!contactor_state) return -1;
    unsigned long now = millis();
    return (settings.contactor_debounce_min*60000UL - (now - contactor_prev_millis));
}
