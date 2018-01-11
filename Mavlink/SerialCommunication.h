#ifndef SERIAL_COMMUNICATION_H
#define SERIAL_COMMUNICATION_H

#include <Arduino.h>  // Serial..

// on Arduino Mega we have several serial ports, so define
// macro to make affinity
#define APM_PORT    Serial2
#define COM_PORT    Serial

void OpenPorts();

#endif
