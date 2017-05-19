#include "movement.h"
#include "variables.h"

#define AltMin       70  // Height at which control begins
#define DistMin      50  // Minimum difference between two distances of the same axis to move.
#define Compensation 800 // Time of inertia compensation in ms



// TODO: meaningless argument
uint16_t CheckPitch(uint16_t Pitch) {
    int16_t Difference = Sensor[0].MeanDistance - Sensor[2].MeanDistance;
    if ( Sensor[4].MeanDistance > AltMin || Sensor[4].MeanDistance == 0 ) {
        if ( abs(Difference) > DistMin ) {
            // Difference greater than 30 between both sensors
            if ( Sensor[0].Close == true ) {
                // Detects the front
                if ( Sensor[2].Close == true ) {
                    // Detects the butt
                    if ( Sensor[0].MeanDistance < Sensor[2].MeanDistance ) {
                        // The front sensor has a shorter distance
                        return( Pitch = ValueRC( Sensor[0].MeanDistance, 1 ) );
                    } else {
                        // The rear sensor has a shorter distance
                        return( Pitch = ValueRC( Sensor[2].MeanDistance, 0 ) );
                    }
                } else {
                    // Detects front, but not rear
                    return( Pitch = ValueRC( Sensor[0].MeanDistance, 1 ) );
                }
            } else {
                // Does not detect the front
                if ( Sensor[2].Close == true ) {
                    // Detects the butt
                    return( Pitch = ValueRC( Sensor[2].MeanDistance, 0 ) );
                } else {
                    // Both have a distance greater than 150
                    return( Pitch = 0 );
                }
            }
        } else if ( Sensor[0].Close == true && Sensor[2].MeanDistance == 0 ) {
            // Detect the forward, and the back to detect nothing, returns 0
            return( Pitch = ValueRC( Sensor[0].MeanDistance, 1 ) );
        } else if ( Sensor[0].MeanDistance == 0 && Sensor[2].Close == true ) {
            // The same but the opposite
            return( Pitch = ValueRC( Sensor[2].MeanDistance, 0 ) );
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
    int16_t Difference = Sensor[1].MeanDistance - Sensor[3].MeanDistance;
    if ( Sensor[4].MeanDistance > AltMin || Sensor[4].MeanDistance == 0 ) {
        if ( abs(Difference) > DistMin ) {
            // Difference greater than 20 between distances
            if ( Sensor[1].Close == true ) {
                // Detects the right
                if ( Sensor[3].Close == true ) {
                    // Detects left
                    if ( Sensor[1].MeanDistance < Sensor[3].MeanDistance ) {
                        // The right sensor has a smaller distance
                        return( Roll = ValueRC( Sensor[1].MeanDistance, 0 ) );
                    } else {
                        // The left sensor has a smaller distance
                        return( Roll = ValueRC( Sensor[3].MeanDistance, 1 ) );
                    }
                } else {
                    // Detects right, but not left
                    return( Roll = ValueRC( Sensor[1].MeanDistance, 0 ) );
                }
            } else {
                //No detecta el derecho
                if ( Sensor[3].Close == true ) {
                    // Detects left
                    return( Roll = ValueRC( Sensor[3].MeanDistance, 1 ) );
                } else {
                    // Both have a distance greater than 150
                    return( Roll = 0 );
                }
            }
        } else if ( Sensor[1].Close == true && Sensor[3].MeanDistance == 0 ) {
            // Detects the right, and the left when it does not detect anything, returns 0
            return( Roll = ValueRC( Sensor[1].MeanDistance, 0 ) );
        } else if ( Sensor[1].MeanDistance == 0 && Sensor[3].Close == true ) {
            // The same but the opposite
            return( Roll = ValueRC( Sensor[3].MeanDistance, 1 ) );
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

    if (PitchOut > 1500 && Sensor[0].Active == false && Sensor[2].Active == false) {
        Sensor[0].Active = true;
    } else if (PitchOut < 1500 && PitchOut != 0 && Sensor[2].Active == false && Sensor[0].Active == false) {
        Sensor[2].Active = true;
    } else if (PitchOut == 0 && Sensor[0].Active == true && Sensor[0].CompensateTime == 0) {
        Sensor[0].CompensateTime = millis();
    } else if (PitchOut == 0 && Sensor[2].Active == true && Sensor[2].CompensateTime == 0) {
        Sensor[2].CompensateTime = millis();
    }

    if (RollOut > 1500 && Sensor[3].Active == false && Sensor[1].Active == false) {
        Sensor[3].Active = true;
    } else if (RollOut < 1500 && RollOut != 0 && Sensor[1].Active == false && Sensor[3].Active == false) {
        Sensor[1].Active = true;
    } else if (RollOut == 0 && Sensor[1].Active == true && Sensor[1].CompensateTime == 0) {
        Sensor[1].CompensateTime = millis();
    } else if (RollOut == 0 && Sensor[3].Active == true && Sensor[3].CompensateTime == 0) {
        Sensor[3].CompensateTime = millis();
    }

    for (int i = 0; i < 4; i++) {
        if (Sensor[i].CompensateTime != 0 && (Sensor[i].CompensateTime + Compensation > millis())) {
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
        } else if (Sensor[i].CompensateTime != 0) {
            switch (i) {
            case 0:
            case 2:
                PitchOut = 0;
                Sensor[i].Active = false;
                Sensor[i].CompensateTime = 0;
                break;
            case 1:
            case 3:
                RollOut = 0;
                Sensor[i].Active = false;
                Sensor[i].CompensateTime = 0;
                break;
            default:
                break;
            }
        }
    }

}