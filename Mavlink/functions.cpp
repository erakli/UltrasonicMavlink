#include "functions.h"
#include "variables.h"
#include "sensor.h"
#include "movement.h"
#include "mavlink_messages.h"



// Task to measure the sensors
void FSensors() {
    MeasureSensors();
    EvalMeanDistances();
    CheckDistances();
}



// Variable used to control the HeartBeat sent every second
unsigned long HeartbeatTime = 0;

// Task responsible for sending a HeartBeat every second
void FHeartBeat() {
    if ( (millis() - HeartbeatTime) > 1000 ) {
        HeartbeatTime = millis();
        HeartBeat();
    }
}



uint16_t PitchOutTemp = 0;
uint16_t RollOutTemp  = 0;
uint8_t n = 0;

// Task that sends the motion commands according to the distances detected by the sensors
void FRCOverride() {
    // TODO: temp assignment
    // Pitch = 0;
    // Roll = 0;
    Pitch  = CheckPitch(Pitch);
    Roll   = CheckRoll(Roll);     // TODO: temp commented

    CompensationInertia();

    // TODO: describe this moment    
    if ( Pitch != PitchOutTemp || Roll != RollOutTemp ) {
        n = 0;
        PitchOutTemp = Pitch;
        RollOutTemp  = Roll;
    } else {
        n += 1;
        if (n == 4) {
            RollOut = RollOutTemp;
            PitchOut = PitchOutTemp;
            RCOverride(PitchOut, RollOut);
        }
    }
}

