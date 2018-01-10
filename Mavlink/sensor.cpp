#include "sensor.h"
#include "variables.h"
#include "SerialCommunication.h"


#ifdef DEBUG_SENSORS
unsigned long last_time = 0;
#define TIME_DELTA 2000
#endif

/* Initialization of the sensor pins. Library "NewPing"
   NewPing NAME (Trigger, Echo, MAXDIST);
   The value of MAXDIST is the maximum distance the library measures.
   If any Echo returns a value greater than said distance, it is automatically discarded
*/

#define MIN_DIST 300
#define MIN_HEIGHT 100

// Distance from which control begins to act
#define CONTROL_DISTANCE_MIN  100 

/*  Sensor0 = Front
    Sensor1 = Right
    Sensor2 = Rear
    Sensor3 = Left */

/* Initialization of the sensor pins. Library "NewPing"
   NewPing NAME (Trigger, Echo, MAXDIST);
   The value of MAXDIST is the maximum distance the library measures.
   If any Echo returns a value greater than said distance, it is automatically discarded*/

NewPing sonars[] = {
    NewPing(4, 5, MIN_DIST),
    NewPing(8, 9, MIN_DIST),
    NewPing(24, 25, MIN_DIST),
    NewPing(27, 28, MIN_DIST),
    NewPing(46, 47, MIN_DIST)
};



#ifdef OLD_COMPILER
Sensor::Sensor() 
    : meanDistance(0), isClose(false), isActive(false), compensateTime(0) 
{
    for (uint8_t i = 0; i < NDistances; i++) {
        distances[i] = 0;
    }
}
#endif

void Sensor::EvalMeanDist() {
    int total = 0;
    uint8_t num = 0;
    for (uint8_t i = 0; i < NDistances; i++) {
        // NB: second statement is meaningless
        if (distances[i] != 0  && distances[i] < MIN_DIST) {
            total += distances[i];
            num += 1;
        }
    }

    if (num > 3) {
        meanDistance = total / num;
    } else {
        meanDistance = 0;
    }
}

void Sensor::CheckDistance() {
    isClose = (meanDistance != 0 && meanDistance < CONTROL_DISTANCE_MIN);
}


// Shift each array of distances on one position to the right
void Sensor::ShiftDistancesArray() {
    for (uint8_t i = NDistances - 1; i > 0; i--) {
        distances[i] = distances[i - 1];
    }
}


// The sensors are measured, and placed in position 0 of each array
void MeasureSensors() {
    for (uint8_t i = 0; i < NSensors; i++) {
        sensors[i].ShiftDistancesArray();
        sensors[i].distances[0] = sonars[i].ping_cm();
    }
}



// The average of all distances is performed. The 0 are discarded
void EvalMeanDistances() {
    for (uint8_t i = 0; i < NSensors; i++) {
        sensors[i].EvalMeanDist();
    }
    
#ifdef DEBUG_SENSORS
    if (millis() - last_time > TIME_DELTA)
    {
        COM_PORT.print("\n\rDistances:\n\r");
        String names[] = { "front", "right", "back", "left", "bottom" };

        for (uint8_t i = 0; i < NSensors; i++) {
            COM_PORT.print(names[i]);
            COM_PORT.print(" = ");
            COM_PORT.print(sensors[i].meanDistance);
            COM_PORT.print(" (");
            COM_PORT.print(sensors[i].isClose);
            COM_PORT.print("), ");
            // COM_PORT.print(" ), raw = ");
            // COM_PORT.print(sensors[i].distances[0]); COM_PORT.print(", ");
            // COM_PORT.print(sensors[i].distances[1]); COM_PORT.print(", ");
            // COM_PORT.print(sensors[i].distances[2]); COM_PORT.print(", ");
            // COM_PORT.print(sensors[i].distances[3]); COM_PORT.print(", ");
            // COM_PORT.print(sensors[i].distances[4]); COM_PORT.print(", ");
            // COM_PORT.print("\n\r");
        }

        COM_PORT.print("\n\r");
        last_time = millis();
    }
#endif
}



// Check if the mean obtained is below the threshold.
void CheckDistances() {
    // Minimum of 10 for distance. There are measurement errors
    for (uint8_t i = 0; i < NSensors; i++) {
        sensors[i].CheckDistance();
    }
}

