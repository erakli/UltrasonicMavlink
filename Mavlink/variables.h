#ifndef VARIABLES_H
#define VARIABLES_H

#include <stdint.h> // uint16_t, etc
#include "sensor.h"

// Variables used to send only one RCOverride at a time
// to be modified, and not to saturate the redundant order controller
extern uint16_t calculatedPitch;
extern uint16_t calculatedRoll;
extern uint16_t pitchOut;
extern uint16_t rollOut;

// The variables of each sensor are created
extern Sensors sensors;

#endif
