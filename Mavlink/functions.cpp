#include "functions.h"

#include <Arduino.h>    // millis()
#include "defines.h"
#include "constants.h"
#include "variables.h"
#include "movement.h"
#include "mavlink_messages.h"

#ifdef DEBUG_RC_COMMANDS
#include "SerialCommunication.h"
#endif

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
    pitch  = CheckChannel(Channel_Pitch);
    roll   = CheckChannel(Channel_Roll);

#ifdef DEBUG_RC_COMMANDS
    static unsigned long lastTime = 0;
    if (millis() - lastTime > 0) {
        COM_PORT.print("pitch = "); 
        COM_PORT.println(pitch);
        COM_PORT.print("roll = "); 
        COM_PORT.println(roll);
        COM_PORT.println("\r\n");

        lastTime = millis();
    }
#endif

    // NB: commented
    // CompensationInertia();

    static uint16_t pitchOutTemp = 0;
    static uint16_t rollOutTemp  = 0;
    static uint8_t n = 0;
   
    if ( pitch == pitchOutTemp && roll == rollOutTemp ) {
        if (n > STABLE_CHANNEL_VALUE_COUNT) {
            rollOut = rollOutTemp;
            pitchOut = pitchOutTemp;
            RCOverride();
        }
        n++;
    } else {
        n = 0;
        pitchOutTemp = pitch;
        rollOutTemp  = roll;
    }
}

