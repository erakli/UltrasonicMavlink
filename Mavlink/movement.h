#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <Arduino.h>  // uint16_t

// TODO: meaningless argument
uint16_t CheckPitch(uint16_t Pitch);

// TODO: meaningless argument
uint16_t CheckRoll(uint16_t Roll);

// Returns an output value depending on the distance
// The greater the distance, the lower the need for movement.
// The variable "Increase" is to know in which direction it is.
uint16_t ValueRC( uint16_t Distance, bool Increase );

void CompensationInertia();

#endif
