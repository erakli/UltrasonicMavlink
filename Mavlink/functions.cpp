#include "functions.h"

#include <Arduino.h>    // millis()
#include "defines.h"
#include "constants.h"
#include "variables.h"
#include "movement.h"
#include "mavlink_messages.h"

#include "serial_communication.h"


// Task responsible for sending a HeartBeat every second
void FHeartBeat() {
#if HEARTBEAT_MESSAGE
    // Variable used to control the HeartBeat sent every second
    static unsigned long lastTime = 0;

    if ( (millis() - lastTime) > HEARTBEAT_OUTPUT_TIME ) {
        HeartBeat();
        lastTime = millis();
    }
#endif
}



// Task that sends the motion commands according to the distances detected 
// by the sensors
void FRCOverride() {
#if ENABLE_RC_CONTROL
    pitch  = CheckChannel(Channel_Pitch);
    roll   = CheckChannel(Channel_Roll);

#if DEBUG_RC_COMMANDS
    static unsigned long lastTime = 0;
    if (millis() - lastTime > RC_COMMANDS_OUTPUT_TIME) {
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
            RCOverride(rollOut, pitchOut);
        }
        n++;
    } else {
        n = 0;
        pitchOutTemp = pitch;
        rollOutTemp  = roll;
    }
#endif
}
