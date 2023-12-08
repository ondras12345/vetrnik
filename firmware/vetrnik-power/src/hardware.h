#pragma once
/* Switching from Arduino's pin manipulation functions to gpio.h:
 *
 * Before
 * RAM:   [=====     ]  47.8% (used 489 bytes from 1024 bytes)
 * Flash: [==========]  103.2% (used 7928 bytes from 7680 bytes)
 *
 * After:
 * RAM:   [=====     ]  47.4% (used 485 bytes from 1024 bytes)
 * Flash: [========= ]  94.8% (used 7282 bytes from 7680 bytes)
 */
#include <avr/io.h>
#include <gpio.h>

#define pin_REL1        B, 0, 0 // PB0
#define pin_REL2        C, 5, 5 // PC5
#define pin_REL3        C, 4, 4 // PC4
#define pin_REL4        B, 5, 5 // PB5
#define pin_FAN         D, 5, 5 // PD5, cannot do PWM on ATmega8
#define pin_SHORT       D, 6, 6 // PD6, inverted

// See Hbridge.h
#define pin_PWM1        B, 1, 1 // PB1
#define pin_PWM2        B, 2, 2 // PB2
#define pin_SD          D, 7, 7 // PD7, inverted

#define pin_VSENSE      C, 0, 0 // PC0
#define pin_ACS712      C, 1, 1 // PC1
#define pin_ACS712_2    C, 2, 2 // PC2
#define pin_NTC1        C, 3, 3 // PC3

// RPM is connected to two pins, but only pin_RPM_1 is used.
#define pin_RPM_1       D, 2, 2 // PD2, INT0
#define pin_RPM_2       D, 4, 4 // PD4
#define pin_ENABLE      D, 3, 3 // PD3, inverted
