#include "functions.h"

#include <Arduino.h>    // millis()
// #include <PID_v1.h>
#include "defines.h"
#include "constants.h"
#include "variables.h"
#include "movement.h"
#include "mavlink_messages.h"

#include "SerialCommunication.h"


// Task responsible for sending a HeartBeat every second
void FHeartBeat() {
    // Variable used to control the HeartBeat sent every second
    static unsigned long lastTime = 0;

    if ( (millis() - lastTime) > HEARTBEAT_OUTPUT_TIME ) {
        HeartBeat();
        lastTime = millis();
    }
}



// Task that sends the motion commands according to the distances detected 
// by the sensors
void FRCOverride() {
    pitch  = CheckChannel(Channel_Pitch);
    roll   = CheckChannel(Channel_Roll);

#ifdef DEBUG_RC_COMMANDS
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
}



// //Define Variables we'll be connecting to
// double Setpoint, Input, Output;

// //Specify the links and initial tuning parameters
// PID myPID(&Input, &Output, &Setpoint, PID_P, PID_I, PID_D, DIRECT);


// void TakeOffInit(uint16_t height) {
//     //initialize the variables we're linked to
//     Input = sensors[4].meanDistance;
//     Setpoint = height;

//     //turn the PID on
//     myPID.SetMode(AUTOMATIC);
//     myPID.SetOutputLimits(1300, 1600);
// }

// bool TakeOffCheck() {
//     // if (abs(Setpoint - sensors[4].meanDistance) < TAKE_OFF_EPSILON)
//     //     return true;

//     //initialize the variables we're linked to
//     Input = sensors[4].meanDistance;
//     myPID.Compute();

//     RCOverride(0, 0, Output, 0);
//     // COM_PORT.print(sensors[4].meanDistance);
//     // COM_PORT.print(", ");
//     // COM_PORT.println(Output);

//     return false;
// }
