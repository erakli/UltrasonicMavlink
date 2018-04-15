#ifndef PINS_H
#define PINS_H

#include "defines.h"

// Serial ---------------------------------------------------------------

#define APM_RX  2
#define APM_TX  3

// LED ------------------------------------------------------------------

#define LED_PIN 13

// Sensors --------------------------------------------------------------

#define FRONT_TRIGGER   4
#define FRONT_ECHO      5

#define RIGHT_TRIGGER   6
#define RIGHT_ECHO      7

#define BACK_TRIGGER    8
#define BACK_ECHO       9

#define LEFT_TRIGGER    10
#define LEFT_ECHO       11

#define BOTTOM_TRIGGER  12
#define BOTTOM_ECHO     13

#if USE_PINS_AS_POWER
    #define FRONT_POWER     A0
    #define FRONT_GROUND    A1

    #define RIGHT_POWER     A2
    #define RIGHT_GROUND    A3

    #define BACK_POWER      A4
    #define BACK_GROUND     A5

    #define LEFT_POWER      A6
    #define LEFT_GROUND     A7

    // #define BOTTOM_POWER    A0
    // #define BOTTOM_GROUND   A1
#endif

#endif