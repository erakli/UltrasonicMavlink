#include "sensor.h"

#include <NewPing.h>
#include "variables.h"

#ifdef DEBUG_SENSORS
#include "SerialCommunication.h"

const uint16_t TIME_DELTA = 2000;
#endif

#define MIN_DIST 300
#define MIN_HEIGHT 100

// Distance from which control begins to act
#define CONTROL_DISTANCE_MIN  100 

/* Initialization of the sensor pins. Library "NewPing"
   NewPing NAME (Trigger, Echo, MAXDIST);
   The value of MAXDIST is the maximum distance the library measures.
   If any Echo returns a value greater than said distance, it is automatically discarded*/

NewPing sonars[] = {
    NewPing(4, 5, MIN_DIST),    // Front
    NewPing(8, 9, MIN_DIST),    // Right
    NewPing(24, 25, MIN_DIST),  // Rear
    NewPing(27, 28, MIN_DIST),  // Left
    NewPing(46, 47, MIN_DIST)   // Bottom
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



// class Sensors --------------------------------------------------------

void Sensors::MakeMeasureCycle() {
    MeasureSensors();
    EvalMeanDistances();
    CheckDistances();

    Print();
}


// The sensors are measured, and placed in position 0 of each array
void Sensors::MeasureSensors() {
    for (uint8_t i = 0; i < NSensors; i++) {
        sensors_[i].ShiftDistancesArray();
        sensors_[i].distances[0] = sonars[i].ping_cm();
    }
}


// The average of all distances is performed. The 0 are discarded
void Sensors::EvalMeanDistances() {
    for (uint8_t i = 0; i < NSensors; i++) {
        sensors_[i].EvalMeanDist();
    }
}


// Check if the mean obtained is below the threshold.
void Sensors::CheckDistances() {
    // Minimum of 10 for distance. There are measurement errors
    for (uint8_t i = 0; i < NSensors; i++) {
        sensors_[i].CheckDistance();
    }
}


void Sensors::Print() {
#ifdef DEBUG_SENSORS
    static unsigned long lastTime = 0;

    if (millis() - lastTime > TIME_DELTA)
    {
        COM_PORT.print("\n\rDistances:\n\r");
        String names[] = { "front", "right", "back", "left", "bottom" };

        for (uint8_t i = 0; i < NSensors; i++) {
            COM_PORT.print(names[i]);
            COM_PORT.print(" = ");
            COM_PORT.print(sensors_[i].meanDistance);
            COM_PORT.print(" (");
            COM_PORT.print(sensors_[i].isClose);
            COM_PORT.print("), ");
            // COM_PORT.print(" ), raw = ");
            // COM_PORT.print(sensors_[i].distances[0]); COM_PORT.print(", ");
            // COM_PORT.print(sensors_[i].distances[1]); COM_PORT.print(", ");
            // COM_PORT.print(sensors_[i].distances[2]); COM_PORT.print(", ");
            // COM_PORT.print(sensors_[i].distances[3]); COM_PORT.print(", ");
            // COM_PORT.print(sensors_[i].distances[4]); COM_PORT.print(", ");
            // COM_PORT.print("\n\r");
        }

        COM_PORT.print("\n\r");
        lastTime = millis();
    }
#endif
}


Sensor& Sensors::operator[](uint8_t idx) {
    return sensors_[idx];
}
