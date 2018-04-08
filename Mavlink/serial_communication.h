#ifndef SERIAL_COMMUNICATION_H
#define SERIAL_COMMUNICATION_H

#include <Arduino.h>  // Serial..
#include "defines.h"

#if defined(__AVR_ATmega328P__)
    #define APM_PORT Serial
    #define COM_PORT Serial
#else
    // on Arduino Mega we have several serial ports, so define
    // macro to make affinity
    #define APM_PORT    Serial2
    #define COM_PORT    Serial1
#endif

void OpenPorts();

#endif
