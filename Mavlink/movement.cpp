#include "movement.h"
#include "variables.h"

#define AltMin       70  // Height at which control begins
#define DistMin      50  // Minimum difference between two distances of the same axis to move.
#define Compensation 800 // Time of inertia compensation in ms



// TODO: meaningless argument
uint16_t CheckPitch(uint16_t Pitch) {
    int16_t Difference = sensors[0].meanDistance - sensors[2].meanDistance;
    if ( sensors[4].meanDistance > AltMin || sensors[4].meanDistance == 0 ) {
        if ( abs(Difference) > DistMin ) {
            // Difference greater than 30 between both sensors
            if ( sensors[0].isClose == true ) {
                // Detects the front
                if ( sensors[2].isClose == true ) {
                    // Detects the butt
                    if ( sensors[0].meanDistance < sensors[2].meanDistance ) {
                        // The front sensor has a shorter distance
                        return( Pitch = ValueRC( sensors[0].meanDistance, 1 ) );
                    } else {
                        // The rear sensor has a shorter distance
                        return( Pitch = ValueRC( sensors[2].meanDistance, 0 ) );
                    }
                } else {
                    // Detects front, but not rear
                    return( Pitch = ValueRC( sensors[0].meanDistance, 1 ) );
                }
            } else {
                // Does not detect the front
                if ( sensors[2].isClose == true ) {
                    // Detects the butt
                    return( Pitch = ValueRC( sensors[2].meanDistance, 0 ) );
                } else {
                    // Both have a distance greater than 150
                    return( Pitch = 0 );
                }
            }
        } else if ( sensors[0].isClose == true && sensors[2].meanDistance == 0 ) {
            // Detect the forward, and the back to detect nothing, returns 0
            return( Pitch = ValueRC( sensors[0].meanDistance, 1 ) );
        } else if ( sensors[0].meanDistance == 0 && sensors[2].isClose == true ) {
            // The same but the opposite
            return( Pitch = ValueRC( sensors[2].meanDistance, 0 ) );
        } else {
            // Does not detect any. Both at 0
            return( Pitch = 0 );
        }
    } else {
        return( Pitch = 0 );
    }
}



// TODO: meaningless argument
uint16_t CheckRoll(uint16_t Roll) {
    int16_t Difference = sensors[1].meanDistance - sensors[3].meanDistance;
    if ( sensors[4].meanDistance > AltMin || sensors[4].meanDistance == 0 ) {
        if ( abs(Difference) > DistMin ) {
            // Difference greater than 20 between distances
            if ( sensors[1].isClose == true ) {
                // Detects the right
                if ( sensors[3].isClose == true ) {
                    // Detects left
                    if ( sensors[1].meanDistance < sensors[3].meanDistance ) {
                        // The right sensor has a smaller distance
                        return( Roll = ValueRC( sensors[1].meanDistance, 0 ) );
                    } else {
                        // The left sensor has a smaller distance
                        return( Roll = ValueRC( sensors[3].meanDistance, 1 ) );
                    }
                } else {
                    // Detects right, but not left
                    return( Roll = ValueRC( sensors[1].meanDistance, 0 ) );
                }
            } else {
                //No detecta el derecho
                if ( sensors[3].isClose == true ) {
                    // Detects left
                    return( Roll = ValueRC( sensors[3].meanDistance, 1 ) );
                } else {
                    // Both have a distance greater than 150
                    return( Roll = 0 );
                }
            }
        } else if ( sensors[1].isClose == true && sensors[3].meanDistance == 0 ) {
            // Detects the right, and the left when it does not detect anything, returns 0
            return( Roll = ValueRC( sensors[1].meanDistance, 0 ) );
        } else if ( sensors[1].meanDistance == 0 && sensors[3].isClose == true ) {
            // The same but the opposite
            return( Roll = ValueRC( sensors[3].meanDistance, 1 ) );
        } else {
            // Does not detect any. Both at 0
            return( Roll = 0 );
        }
    } else {
        return( Roll = 0 );
    }
}



// Returns an output value depending on the distance
// The greater the distance, the lower the need for movement.
// The variable "Increase" is to know in which direction it is.
uint16_t ValueRC( uint16_t Distance, bool Increase ) {
    if ( Distance < 30 ) {
        if ( Increase == true ) {
            return( 1700 );
        } else {
            return( 1300 );
        }
    } else if ( Distance < 90 ) {
        if ( Increase == true ) {
            return( 1675 );
        } else {
            return( 1325 );
        }
    } else if ( Distance < 150 ) {
        if ( Increase == true ) {
            return( 1650 );
        } else {
            return( 1350 );
        }
    }
}



void CompensationInertia() {

    if (PitchOut > 1500 && sensors[0].isActive == false && sensors[2].isActive == false) {
        sensors[0].isActive = true;
    } else if (PitchOut < 1500 && PitchOut != 0 && sensors[2].isActive == false && sensors[0].isActive == false) {
        sensors[2].isActive = true;
    } else if (PitchOut == 0 && sensors[0].isActive == true && sensors[0].compensateTime == 0) {
        sensors[0].compensateTime = millis();
    } else if (PitchOut == 0 && sensors[2].isActive == true && sensors[2].compensateTime == 0) {
        sensors[2].compensateTime = millis();
    }

    if (RollOut > 1500 && sensors[3].isActive == false && sensors[1].isActive == false) {
        sensors[3].isActive = true;
    } else if (RollOut < 1500 && RollOut != 0 && sensors[1].isActive == false && sensors[3].isActive == false) {
        sensors[1].isActive = true;
    } else if (RollOut == 0 && sensors[1].isActive == true && sensors[1].compensateTime == 0) {
        sensors[1].compensateTime = millis();
    } else if (RollOut == 0 && sensors[3].isActive == true && sensors[3].compensateTime == 0) {
        sensors[3].compensateTime = millis();
    }

    for (int i = 0; i < 4; i++) {
        if (sensors[i].compensateTime != 0 && (sensors[i].compensateTime + Compensation > millis())) {
            switch (i) {
            case 0:
                Pitch = 1300;
                break;
            case 1:
                Roll = 1700;
                break;
            case 2:
                Pitch = 1700;
                break;
            case 3:
                Roll = 1300;
                break;
            default:
                break;
            }
        } else if (sensors[i].compensateTime != 0) {
            switch (i) {
            case 0:
            case 2:
                PitchOut = 0;
                sensors[i].isActive = false;
                sensors[i].compensateTime = 0;
                break;
            case 1:
            case 3:
                RollOut = 0;
                sensors[i].isActive = false;
                sensors[i].compensateTime = 0;
                break;
            default:
                break;
            }
        }
    }

}