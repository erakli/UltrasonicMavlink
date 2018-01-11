#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <stdint.h> // uint16_t, etc
#include "sensor.h"

enum Channels {
    Channel_Pitch,
    Channel_Roll
};

enum Directions {
    Direction_Front,
    Direction_Right,
    Direction_Back,
    Direction_Left
};

uint16_t CheckChannel(Channels channel);
bool CheckHeight();
uint16_t GetRCValueForSensors(const Sensor &sensorA, const Sensor &sensorB, Directions dirA, Directions dirB);

// Returns an output value depending on the distance
// The greater the distance, the lower the need for movement.
// The variable "direction" is to know in which direction it is.
uint16_t ValueRC( uint16_t distance, Directions direction );

void CompensationInertia();

#endif
