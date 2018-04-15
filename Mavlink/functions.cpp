#include "functions.h"

#include <Arduino.h>    // millis()
#include "defines.h"
#if SOUND_INDICATION
    #include <NewTone.h>
    #define TONE_PIN 2
#endif
#include "constants.h"
#include "movement.h"
#include "mavlink_messages.h"
#include "serial_communication.h"

uint16_t desiredPitch = 0;
uint16_t desiredRoll  = 0;
uint8_t sameDesiredManeuverCount = 0;


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
    int16_t calculatedPitch  = CheckChannel(Channel_Pitch);
    int16_t calculatedRoll   = CheckChannel(Channel_Roll);

    // NB: commented
    // CompensationInertia();
   
    if (calculatedPitch == desiredPitch && calculatedRoll == desiredRoll &&
        (calculatedPitch != 0 || calculatedRoll != 0)) {
        if (STABLE_CHANNEL_VALUE_COUNT < sameDesiredManeuverCount) {
            uint16_t pitchOut = ZERO_RC_VALUE + desiredPitch;
            uint16_t rollOut = ZERO_RC_VALUE + desiredRoll;
        #if SOUND_INDICATION
            if (pitchOut != 0 && rollOut != 0)
                NewTone(TONE_PIN, 3000, 50);
            else if (pitchOut != 0)
                NewTone(TONE_PIN, 2000, 50);
            else if (rollOut != 0)
                NewTone(TONE_PIN, 1000, 50);
        #endif
        #if DEBUG_RC_COMMANDS
            static unsigned long lastTime = 0;
            if (RC_COMMANDS_OUTPUT_TIME < millis() - lastTime) {
                COM_PORT.print(calculatedPitch);
                COM_PORT.print(",   ");
                COM_PORT.print(calculatedRoll);
                COM_PORT.println();

                lastTime = millis();
            }
        #endif
        #if ENABLE_RC_CONTROL
            RCOverride(rollOut, pitchOut);
        #endif

        // NOTE: возможно стоит обнулять sameDesiredManeuverCount
        } else {
            sameDesiredManeuverCount++;
        }
    } else {
        sameDesiredManeuverCount = 0;
        desiredPitch = calculatedPitch;
        desiredRoll  = calculatedRoll;
    }
}
