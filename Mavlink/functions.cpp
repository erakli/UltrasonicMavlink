#include "functions.h"

#include <Arduino.h>    // millis()
#include "defines.h"
#if !USE_MAVLINK && DEBUG
    #include <NewTone.h>
    #define TONE_PIN 2
#endif
#include "constants.h"
#include "variables.h"
#include "movement.h"
#include "mavlink_messages.h"

#include "serial_communication.h"

uint16_t pitchOutTemp = 0;
uint16_t rollOutTemp  = 0;
uint8_t n = 0;

// Task responsible for sending a HeartBeat every second
void FHeartBeat() {
#if HEARTBEAT_MESSAGE
    // Variable used to control the HeartBeat sent every second
    static unsigned long lastTime = 0;

    if (HEARTBEAT_OUTPUT_TIME < millis() - lastTime) {
        HeartBeat();
        lastTime = millis();
    }
#endif
}



// Task that sends the motion commands according to the distances detected 
// by the sensors
void FRCOverride() {
    calculatedPitch  = CheckChannel(Channel_Pitch);
    calculatedRoll   = CheckChannel(Channel_Roll);

#if DEBUG_RC_COMMANDS
    static unsigned long lastTime = 0;
    if (RC_COMMANDS_OUTPUT_TIME < millis() - lastTime) {
        COM_PORT.print("calculatedPitch = "); 
        COM_PORT.println(calculatedPitch);
        COM_PORT.print("calculatedRoll = "); 
        COM_PORT.println(calculatedRoll);
        COM_PORT.println();

        lastTime = millis();
    }
#endif

    // NB: commented
    // CompensationInertia();
   
    if ( calculatedPitch == pitchOutTemp && calculatedRoll == rollOutTemp ) {
        if (n > STABLE_CHANNEL_VALUE_COUNT) {
            rollOut = rollOutTemp;
            pitchOut = pitchOutTemp;
        #if ENABLE_RC_CONTROL
            RCOverride(rollOut, pitchOut);
        #elif DEBUG
            NewTone(TONE_PIN, 1500, 100);
        #endif
        }
        n++;
    } else {
        n = 0;
        pitchOutTemp = calculatedPitch;
        rollOutTemp  = calculatedRoll;
    }
}
