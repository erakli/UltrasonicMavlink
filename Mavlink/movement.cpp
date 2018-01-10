#include "movement.h"

#include <Arduino.h>    // millis()
#include "variables.h"

#define ALT_MIN             70  // Height at which control begins
#define DIST_MIN            50  // Minimum difference between two distances of the same axis to move.
#define COMPENSATION_TIME   800 // Time of inertia compensation in ms


uint16_t CheckPitch() {
    if ( sensors[4].meanDistance > ALT_MIN || sensors[4].meanDistance == 0 ) {
        int16_t difference = sensors[0].meanDistance - sensors[2].meanDistance;
        if ( abs(difference) > DIST_MIN ) {
            // difference greater than 30 between both sensors
            if ( sensors[0].isClose ) {
                // Detects the front
                if ( sensors[2].isClose ) {
                    // Detects the butt
                    if ( sensors[0].meanDistance < sensors[2].meanDistance ) {
                        // The front sensor has a shorter distance
                        return ValueRC( sensors[0].meanDistance, true );
                    } else {
                        // The rear sensor has a shorter distance
                        return ValueRC( sensors[2].meanDistance, false );
                    }
                } else {
                    // Detects front, but not rear
                    return ValueRC( sensors[0].meanDistance, true );
                }
            } else {
                // Does not detect the front
                if ( sensors[2].isClose ) {
                    // Detects the butt
                    return ValueRC( sensors[2].meanDistance, false );
                } else {
                    // Both have a distance greater than 150
                    return 0;
                }
            }
        } else if ( sensors[0].isClose && sensors[2].meanDistance == 0 ) {
            // Detect the forward, and the back to detect nothing, returns 0
            return ValueRC( sensors[0].meanDistance, true );
        } else if ( sensors[0].meanDistance == 0 && sensors[2].isClose ) {
            // The same but the opposite
            return ValueRC( sensors[2].meanDistance, false );
        } else {
            // Does not detect any. Both at 0
            return 0;
        }
    } else {
        return 0;
    }
}


uint16_t CheckRoll() {
    if ( sensors[4].meanDistance > ALT_MIN || sensors[4].meanDistance == 0 ) {
        int16_t difference = sensors[1].meanDistance - sensors[3].meanDistance;
        if ( abs(difference) > DIST_MIN ) {
            // difference greater than 20 between distances
            if ( sensors[1].isClose ) {
                // Detects the right
                if ( sensors[3].isClose ) {
                    // Detects left
                    if ( sensors[1].meanDistance < sensors[3].meanDistance ) {
                        // The right sensor has a smaller distance
                        return ValueRC( sensors[1].meanDistance, false );
                    } else {
                        // The left sensor has a smaller distance
                        return ValueRC( sensors[3].meanDistance, true );
                    }
                } else {
                    // Detects right, but not left
                    return ValueRC( sensors[1].meanDistance, false );
                }
            } else {
                // Does not detect the right
                if ( sensors[3].isClose ) {
                    // Detects left
                    return ValueRC( sensors[3].meanDistance, true );
                } else {
                    // Both have a distance greater than 150
                    return 0;
                }
            }
        } else if ( sensors[1].isClose && sensors[3].meanDistance == 0 ) {
            // Detects the right, and the left when it does not detect anything, returns 0
            return ValueRC( sensors[1].meanDistance, false );
        } else if ( sensors[1].meanDistance == 0 && sensors[3].isClose ) {
            // The same but the opposite
            return ValueRC( sensors[3].meanDistance, true );
        } else {
            // Does not detect any. Both at 0
            return 0;
        }
    } else {
        return 0;
    }
}



// Returns an output value depending on the distance
// The greater the distance, the lower the need for movement.
// The variable "increase" is to know in which direction it is.
uint16_t ValueRC( uint16_t distance, bool increase ) {
    if ( distance < 30 ) {
        if ( increase ) {
            return( 1700 );
        } else {
            return( 1300 );
        }
    } else if ( distance < 90 ) {
        if ( increase ) {
            return( 1675 );
        } else {
            return( 1325 );
        }
    } else if ( distance < 150 ) {
        if ( increase ) {
            return( 1650 );
        } else {
            return( 1350 );
        }
    }
}



void CompensationInertia() {

    if (pitchOut > 1500 && (!sensors[0].isActive) && (!sensors[2].isActive)) {
        sensors[0].isActive = true;
    } else if (pitchOut < 1500 && pitchOut != 0 && (!sensors[2].isActive) && (!sensors[0].isActive)) {
        sensors[2].isActive = true;
    } else if (pitchOut == 0 && sensors[0].isActive && sensors[0].compensateTime == 0) {
        sensors[0].compensateTime = millis();
    } else if (pitchOut == 0 && sensors[2].isActive && sensors[2].compensateTime == 0) {
        sensors[2].compensateTime = millis();
    }

    if (rollOut > 1500 && (!sensors[3].isActive) && (!sensors[1].isActive)) {
        sensors[3].isActive = true;
    } else if (rollOut < 1500 && rollOut != 0 && (!sensors[1].isActive) && (!sensors[3].isActive)) {
        sensors[1].isActive = true;
    } else if (rollOut == 0 && sensors[1].isActive && sensors[1].compensateTime == 0) {
        sensors[1].compensateTime = millis();
    } else if (rollOut == 0 && sensors[3].isActive && sensors[3].compensateTime == 0) {
        sensors[3].compensateTime = millis();
    }

    for (int i = 0; i < 4; i++) {
        if (sensors[i].compensateTime != 0 && (sensors[i].compensateTime + COMPENSATION_TIME > millis())) {
            switch (i) {
                case 0:
                    pitch = 1300;
                    break;
                case 1:
                    roll = 1700;
                    break;
                case 2:
                    pitch = 1700;
                    break;
                case 3:
                    roll = 1300;
                    break;
                default:
                    break;
            }
        } else if (sensors[i].compensateTime != 0) {
            switch (i) {
                case 0:
                case 2:
                    pitchOut = 0;
                    sensors[i].isActive = false;
                    sensors[i].compensateTime = 0;
                    break;
                case 1:
                case 3:
                    rollOut = 0;
                    sensors[i].isActive = false;
                    sensors[i].compensateTime = 0;
                    break;
                default:
                    break;
            }
        }
    }

}