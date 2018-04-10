#ifndef SERIAL_COMMUNICATION_H
#define SERIAL_COMMUNICATION_H

#include <Arduino.h>  // Serial..
#include "defines.h"

#if defined(__AVR_ATmega2560__)
    // on Arduino Mega we have several serial ports, so define
    // macro to make affinity
    #define APM_PORT    Serial2
    #define COM_PORT    Serial1
#else
    #include <SoftwareSerial.h>

    extern SoftwareSerial apmSerial;

    #define APM_PORT apmSerial
    #define COM_PORT Serial
#endif

void OpenPorts();

#endif
