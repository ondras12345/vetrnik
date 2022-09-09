#pragma once

#define pin_REL1        8   // PB0
#define pin_REL2        A5  // PC5
#define pin_REL3        A4  // PC4
#define pin_REL4        13  // PB5
#define pin_FAN         5   // PD5
#define pin_SHORT       6   // PD6, inverted

// See Hbridge.h
//#define pin_PWM1        9   // PB1
//#define pin_PWM2        10  // PB2
//#define pin_SD          7   // PD7, inverted

#define pin_VSENSE      A0  // PC0
#define pin_ACS712      A1  // PC1
#define pin_ACS712_2    A2  // PC2
#define pin_NTC1        A3  // PC3

// RPM is connected to two pins, but only pin_RPM_1 is used.
#define pin_RPM_1       2   // PD2
#define pin_RPM_2       4   // PD4
#define pin_ENABLE      3   // PD3, inverted
