#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <stdint.h> // uint16_t, etc

uint16_t CheckPitch();
uint16_t CheckRoll();

// Returns an output value depending on the distance
// The greater the distance, the lower the need for movement.
// The variable "increase" is to know in which direction it is.
uint16_t ValueRC( uint16_t distance, bool increase );

void CompensationInertia();

#endif
