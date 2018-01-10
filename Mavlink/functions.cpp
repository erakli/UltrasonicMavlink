#include "functions.h"

#include <Arduino.h>    // millis()
#include "variables.h"
#include "movement.h"
#include "mavlink_messages.h"



// Task responsible for sending a HeartBeat every second
void FHeartBeat() {
    // Variable used to control the HeartBeat sent every second
    static unsigned long heartbeatTime = 0;

    if ( (millis() - heartbeatTime) > 1000 ) {
        heartbeatTime = millis();
        HeartBeat();
    }
}



// Task that sends the motion commands according to the distances detected 
// by the sensors
void FRCOverride() {
    pitch  = CheckPitch();
    roll   = CheckRoll();

    CompensationInertia();

    static uint16_t pitchOutTemp = 0;
    static uint16_t rollOutTemp  = 0;
    static uint8_t n = 0;

    // TODO: describe this moment    
    if ( pitch != pitchOutTemp || roll != rollOutTemp ) {
        n = 0;
        pitchOutTemp = pitch;
        rollOutTemp  = roll;
    } else {
        n += 1;
        if (n == 4) {
            rollOut = rollOutTemp;
            pitchOut = pitchOutTemp;
            RCOverride();
        }
    }
}

