#include "LED.h"

#include <Arduino.h> // OUTPUT and other stuff
#include "defines.h"

#ifdef LED_INDICATION
short int led = 13;
bool state = false;
#endif

void LED_init() {
#ifdef LED_INDICATION
    pinMode(led, OUTPUT);
    digitalWrite(led, LOW);
#endif
}

void LED_change_state() {
#ifdef LED_INDICATION
    if (state)
    {
        digitalWrite(led, LOW);
        state = false;
    }
    else
    {
        digitalWrite(led, HIGH);
        state = true;
    }
#endif
}
