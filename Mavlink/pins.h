#ifndef PINS_H
#define PINS_H

#include "defines.h"

// Serial ---------------------------------------------------------------

#define APM_RX  2
#define APM_TX  3

// LED ------------------------------------------------------------------

#define LED_PIN 10

// Sensors --------------------------------------------------------------

#define FRONT_TRIGGER   11
#define FRONT_ECHO      12

#define RIGHT_TRIGGER   8
#define RIGHT_ECHO      9

#define BACK_TRIGGER    24
#define BACK_ECHO       25

#define LEFT_TRIGGER    27
#define LEFT_ECHO       28

#define BOTTOM_TRIGGER  46
#define BOTTOM_ECHO     47

#if USE_PINS_AS_POWER
    #define FRONT_POWER     A0
    #define FRONT_GROUND    A1

    #define RIGHT_POWER     A2
    #define RIGHT_GROUND    A3

    #define BACK_POWER      A4
    #define BACK_GROUND     A5

    #define LEFT_POWER      A6
    #define LEFT_GROUND     A7

    #define BOTTOM_POWER    A0
    #define BOTTOM_GROUND   A1
#endif

#endif