#include "movement.h"

#include <Arduino.h>    // millis()
#include "defines.h"
#include "constants.h"
#include "variables.h"

#if DEBUG_MOVEMENT
#include "serial_communication.h"
#endif


int16_t CheckChannel(Channels channel) {
    if ( !CheckHeight() ) {
#if DEBUG_MOVEMENT
        COM_PORT.println("low height");
#endif
        return 0;
    }

    Sensor sensorA;
    Sensor sensorB;

    Directions dirA;
    Directions dirB;

    switch (channel) {
        case Channel_Pitch:
            sensorA = sensors[0];
            sensorB = sensors[2];
            dirA = Direction_Front;
            dirB = Direction_Back;
        break;

        case Channel_Roll:
            sensorA = sensors[3];
            sensorB = sensors[1];
            dirA = Direction_Left;
            dirB = Direction_Right;
        break;
    }

    return GetRCValueForSensors(sensorA, sensorB, dirA, dirB);
}


bool CheckHeight() {
    return ( MIN_HEIGHT < sensors[4].meanDistance || sensors[4].meanDistance == 0 );
}


int16_t GetRCValueForSensors(  const Sensor &sensorA, const Sensor &sensorB, 
                                Directions dirA, Directions dirB) 
{
    int16_t difference = sensorA.meanDistance - sensorB.meanDistance;
    if ( MIN_DISTANCES_DIFFERENCE < abs(difference) ) {
        if ( sensorA.isClose && sensorB.isClose ) { 
            if ( difference < 0 ) {
                return ValueRC( sensorA.meanDistance, dirB );
            } else {
                return ValueRC( sensorB.meanDistance, dirA );
            }
        }
        
        if ( sensorA.isClose && !sensorB.isClose ) {
            return ValueRC( sensorA.meanDistance, dirB );
        }

        if ( sensorB.isClose ) {
            return ValueRC( sensorB.meanDistance, dirA );
        }
    } else {
        if ( sensorA.isClose && sensorB.meanDistance == 0 ) {
            return ValueRC( sensorA.meanDistance, dirB );
        } 
        
        if ( sensorA.meanDistance == 0 && sensorB.isClose ) {
            return ValueRC( sensorB.meanDistance, dirA );
        }
    }

    return 0;
}


// Returns an output value depending on the distance
// The greater the distance, the lower the need for movement.
// The variable "direction" is to know in which direction it is.
int16_t ValueRC( const uint16_t distance, Directions direction ) {

    // distance range
    static const uint8_t VALUES_SIZE = 3;
    static const uint16_t distances[] = { 
        CONTROL_DISTANCE_MIN / 4, 
        CONTROL_DISTANCE_MIN / 2, 
        CONTROL_DISTANCE_MIN
    };
    static const uint16_t pwm_values[] = { 150, 100, 50 };

    // direction
    static int16_t signs[] = {
        //  Front,  Right,  Back,   Left    
            -1,     1,      1,      -1
    };

    uint8_t distanceRange = FindValue(distance, distances, VALUES_SIZE);
    if (distanceRange < VALUES_SIZE)
        return pwm_values[distanceRange] * signs[direction];

#if DEBUG_MOVEMENT
    COM_PORT.print("ValueRC: distance out of control distance (distance = ");
    COM_PORT.print(distance);
    COM_PORT.print(", distanceRange = ");
    COM_PORT.print(distanceRange);
    COM_PORT.println();
#endif
    return 0;
}


// TODO: rewrite this
// void CompensationInertia() {

//     if (pitchOut > 1500 && (!sensors[0].isActive) && (!sensors[2].isActive)) {
//         sensors[0].isActive = true;
//     } else if (pitchOut < 1500 && pitchOut != 0 && (!sensors[2].isActive) && (!sensors[0].isActive)) {
//         sensors[2].isActive = true;
//     } else if (pitchOut == 0 && sensors[0].isActive && sensors[0].compensateTime == 0) {
//         sensors[0].compensateTime = millis();
//     } else if (pitchOut == 0 && sensors[2].isActive && sensors[2].compensateTime == 0) {
//         sensors[2].compensateTime = millis();
//     }

//     if (rollOut > 1500 && (!sensors[3].isActive) && (!sensors[1].isActive)) {
//         sensors[3].isActive = true;
//     } else if (rollOut < 1500 && rollOut != 0 && (!sensors[1].isActive) && (!sensors[3].isActive)) {
//         sensors[1].isActive = true;
//     } else if (rollOut == 0 && sensors[1].isActive && sensors[1].compensateTime == 0) {
//         sensors[1].compensateTime = millis();
//     } else if (rollOut == 0 && sensors[3].isActive && sensors[3].compensateTime == 0) {
//         sensors[3].compensateTime = millis();
//     }

//     for (int i = 0; i < 4; i++) {
//         if (sensors[i].compensateTime != 0 && (sensors[i].compensateTime + COMPENSATION_TIME > millis())) {
//             switch (i) {
//                 case 0:
//                     pitch = 1300;
//                     break;
//                 case 1:
//                     roll = 1700;
//                     break;
//                 case 2:
//                     pitch = 1700;
//                     break;
//                 case 3:
//                     roll = 1300;
//                     break;
//                 default:
//                     break;
//             }
//         } else if (sensors[i].compensateTime != 0) {
//             switch (i) {
//                 case 0:
//                 case 2:
//                     pitchOut = 0;
//                     sensors[i].isActive = false;
//                     sensors[i].compensateTime = 0;
//                     break;
//                 case 1:
//                 case 3:
//                     rollOut = 0;
//                     sensors[i].isActive = false;
//                     sensors[i].compensateTime = 0;
//                     break;
//                 default:
//                     break;
//             }
//         }
//     }
// }