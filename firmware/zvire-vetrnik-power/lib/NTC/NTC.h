#pragma once

#include <stdint.h>
#include <math.h>

#define ADC_MAX 1023
#define ROOM_TEMPERATURE_KELVIN 298  // 298.15

/**
 * Only tested with Rdiv = 4700 , beta = 3977, R_nom = 10000
 */
class NTC
{
    public:
        NTC(uint16_t Rdiv, uint16_t beta, uint16_t R_nom):
            Rdiv_(Rdiv), beta_(beta), R_nom_(R_nom)
            {};

        uint16_t getC(uint16_t ADC_reading)
        {
            if (ADC_reading == 0) return 0;
            uint32_t R_thermistor = ((uint32_t)(Rdiv_) * ADC_MAX) / ADC_reading - Rdiv_;
            if (R_thermistor == 0) return 65535;  // This shouldn't happen, and log wouldn't like it.

            // Floating point division takes about 200B of FLASH space,
            // got rid of it by using log(a/b) = log(a) - log(b)
            uint16_t temperature_kelvin =
                (uint32_t(beta_) * ROOM_TEMPERATURE_KELVIN) /
                (uint32_t)(beta_ + ROOM_TEMPERATURE_KELVIN * (log(float(R_thermistor)) - log(R_nom_)));

            if (temperature_kelvin < 273) return 0;  // negative temperatures not supported
            return temperature_kelvin - 273;
        }
    private:
        uint16_t Rdiv_;
        uint16_t beta_;
        uint16_t R_nom_;
};
