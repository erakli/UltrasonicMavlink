#include "variables.h"

// Variables used to send only one RCOverride at a time
// to be modified, and not to saturate the redundant order controller
uint16_t Pitch        = 0;
uint16_t Roll         = 0;
uint16_t PitchOut     = 0;
uint16_t RollOut      = 0;


// The variables of each sensor are created
Sensor sensors[NSensors];