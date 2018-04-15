#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <stdint.h> // uint16_t, etc
#include "sensor.h"

enum Channels {
    Channel_Pitch,
    Channel_Roll,
    Channel_Throttle
};

enum Directions {
    Direction_Front,
    Direction_Right,
    Direction_Back,
    Direction_Left
};

int16_t CheckChannel(Channels channel);
bool CheckHeight();
int16_t GetRCValueForSensors(const Sensor &sensorA, const Sensor &sensorB, Directions dirA, Directions dirB);

// Returns an output value depending on the distance
// The greater the distance, the lower the need for movement.
// The variable "direction" is to know in which direction it is.
int16_t ValueRC( const uint16_t distance, Directions direction );


template <typename T, typename IdxType> IdxType FindValue(const T &value, const T values[], const IdxType &size) {
    for (IdxType i = 0; i < size; i++) {
        if (value < values[i])
            return i;
    }

    return size;
}

// void CompensationInertia();

#endif
