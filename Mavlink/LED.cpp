#include "LED.h"

#include <Arduino.h> // OUTPUT and other stuff
#include "defines.h"
#include "pins.h"


void LED_init() {
#if LED_INDICATION
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
#endif
}

void LED_change_state() {
#if LED_INDICATION
    static bool state = false;
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
