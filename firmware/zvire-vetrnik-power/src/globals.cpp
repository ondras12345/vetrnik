#include "globals.h"
#include "error_templates.h"
#include "Hbridge.h"

bool enabled = false;
bool emergency = false;
bool OVP_stop = false;
uint8_t OCP_max_duty = 255;
uint8_t duty = 0;
uint16_t RPM = 0;
uint16_t voltage = 0;
uint16_t current = 0;
uint16_t temperature_heatsink = 0;
uint16_t temperature_rectifier = 0;
uint8_t fan = 0;

mode_t mode = shorted;


void set_duty(uint8_t value)
{
    if (value != duty)
    {
        duty = value;
        if (mode == const_duty)
        {
            Hbridge_set_duty(value);
        }
    }
}


void emergency_stop()
{
    set_mode(shorted);
    emergency = true;
    errm_add(errm_create(&etemplate_emergency));
}
