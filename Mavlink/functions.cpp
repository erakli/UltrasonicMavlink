#include "functions.h"
#include "variables.h"
#include "sensors.h"
#include "movement.h"
#include "mavlink_messages.h"

// Variable used to control the HeartBeat sent every second
unsigned long HeartbeatTime = 0;

uint16_t PitchOutTemp = 0;
uint16_t RollOutTemp  = 0;
uint8_t n = 0;



// Task to measure the sensors
void FSensors() {
    ShiftArrays();
    MeasureSensors();
    MeanDistances();
    CheckDistances();
}



// Task responsible for sending a HeartBeat every second
void FHeartBeat() {
    if ( (millis() - HeartbeatTime) > 1000 ) {
        HeartbeatTime = millis();
        HeartBeat();
    }
}



// Task that sends the motion commands according to the distances detected by the sensors
void FRCOverride() {
    // TODO: temp assignment
    Pitch = 0;
    Roll = 0;
    Pitch  = CheckPitch(Pitch);
    Roll   = CheckRoll(Roll);     // TODO: temp commented

    CompensationInertia();
 
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



// Shift each array of Distances on one position to the right
void ShiftArrays() {
    for (uint8_t i = 0; i < NSensors; i++) {
        for (uint8_t j = NDistances - 1; j > 0; j--) {
            Sensor[i].Distances[j] = Sensor[i].Distances[j - 1];
        }
    }
}
