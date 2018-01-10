#ifndef VARIABLES_H
#define VARIABLES_H

#include <Arduino.h>  // uint16_t
#include "sensor.h"

// Variables used to send only one RCOverride at a time
// to be modified, and not to saturate the redundant order controller
extern uint16_t Pitch;
extern uint16_t Roll;
extern uint16_t PitchOut;
extern uint16_t RollOut;

// The variables of each sensor are created
#define NSensors 5
extern Sensor sensors[];

#endif
