#pragma once
#include <stdint.h>


/**
 * Define const char * tmp containing either "1" or "0",
 * depending on the value of v.
 * @param v a boolean variable or expression.
 */
#define MAKETMP_BOOL(v)                                                     \
    const char * tmp = (v) ? "1" : "0";


/**
 * Define char[] tmp containing string representation of unsigned fixed-point
 * decimal.
 * @param v unsigned integer value (e.g. uint16_t)
 * @param dp position of the decimal point (number of decimal places).
 *           0 produces a number ending with ".0".
 *
 * Edge case: For uint32_t v = -1;, MAKETMP_DECIMAL(v, 10) will output
 * "1.0000000000" instead of the correct "0.4294967295". This only happens if
 * 10 decimal places are requested.
 */
#define MAKETMP_DECIMAL(v, dp)                                              \
    char tmp[3*sizeof(v) + 1 + 1]; /* int + decimal point + '\0; */         \
    /* Type cast of the whole value because of cppcheck. */                 \
    /* I don't think it is really needed. */                                \
    snprintf(tmp, sizeof tmp,                                               \
             "%u.%0" #dp "u",                                               \
             (unsigned int)(v / (unsigned int)(1E ## dp)),                  \
             (unsigned int)(v % (unsigned int)(1E ## dp))                   \
            );


/**
 * Define char[] tmp containing string representation of unsigned int value
 * @param v unsigned integer value (e.g. uint16_t)
 */
#define MAKETMP_UINT(v)                                                     \
    char tmp[3*sizeof(v) + 1];                                              \
    snprintf(tmp, sizeof tmp, "%u", v);


/**
 * @brief Basic non-equality condition.
 * @param var is the name of a variable containing current value.
 *            A matching variable prev_var should contain previously reported
 *            value.
 */
#define COND_NEQ(var) \
    (var != prev_ ## var)


/**
 * @brief Comparison with hysteresis for uint16_t
 * @param var is the name of a variable containing current value.
 *            A matching variable prev_var should contain previously reported
 *            value.
 */
#define COND_HYST(var, hysteresis)                                          \
    /* We need to perform the comparison with signed integers */            \
    (abs((int_fast32_t)var - (int_fast32_t)(prev_ ## var)) > hysteresis)
