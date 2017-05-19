#ifndef SERIAL_COMMUNICATION_H
#define SERIAL_COMMUNICATION_H

#include <Arduino.h>  // Serial..

#include "defines.h"    // for define statements

// on Arduino Mega we have several serial ports, so define
// macro to make affinity
#define APM_PORT    Serial2
#define COM_PORT    Serial1

void openPorts(unsigned long baudrate);

#endif
