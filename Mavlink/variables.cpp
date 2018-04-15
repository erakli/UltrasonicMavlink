#include "variables.h"

// Variables used to send only one RCOverride at a time
// to be modified, and not to saturate the redundant order controller
uint16_t calculatedPitch        = 0;
uint16_t calculatedRoll         = 0;
uint16_t pitchOut     = 0;
uint16_t rollOut      = 0;


// The variables of each sensor are created
Sensors sensors;