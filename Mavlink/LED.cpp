#include "LED.h"

#include <Arduino.h> // OUTPUT and other stuff
#include "defines.h"
#include "pins.h"

#ifdef LED_INDICATION
bool state = false;
#endif

void LED_init() {
#ifdef LED_INDICATION
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
#endif
}

void LED_change_state() {
#ifdef LED_INDICATION
    if (state)
    {
        digitalWrite(LED_PIN, LOW);
        state = false;
    }
    else
    {
        digitalWrite(LED_PIN, HIGH);
        state = true;
    }
#endif
}
