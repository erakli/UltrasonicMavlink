#include "sensors.h"
#include "variables.h"
#include "SerialCommunication.h"



#ifdef DEBUG_SENSORS
unsigned long last_time = 0;
#define TIME_DELTA 400
#endif

#define ControlDistanceMin 150 // Distance from which control begins to act // TODO: changed from 100

/* Initialization of the sensor pins. Library "NewPing"
   NewPing NAME (Trigger, Echo, MAXDIST);
   The value of MAXDIST is the maximum distance the library measures.
   If any Echo returns a value greater than said distance, it is automatically discarded*/

#define MIN_DIST 300
#define MIN_HEIGHT 10      // TODO: changed from 100

NewPing sonar0(4, 5, MIN_DIST);
NewPing sonar1(8, 9, MIN_DIST);
NewPing sonar2(24, 25, MIN_DIST);
NewPing sonar3(27, 28, MIN_DIST);
NewPing sonar4(46, 47, MIN_HEIGHT);



#ifdef OLD_COMPILER
Sensors::Sensors() 
    : MeanDistance(0), Close(false), Active(false), CompensateTime(0) 
{
    for (uint8_t i = 0; i < NDistances; i++) {
        Distances[i] = 0;
    }
}
#endif



// The sensors are measured, and placed in position 0 of each array
void MeasureSensors() {
    Sensor[0].Distances[0] = sonar0.ping_cm();
    Sensor[1].Distances[0] = sonar1.ping_cm();
    Sensor[2].Distances[0] = sonar2.ping_cm();
    Sensor[3].Distances[0] = sonar3.ping_cm();
    Sensor[4].Distances[0] = sonar4.ping_cm();
}



// The average of all distances is performed. The 0 are discarded
void MeanDistances() {
    int Total;
    uint8_t Num;
    for (uint8_t i = 0; i < NSensors; i++) {
        Total = 0;
        Num = 0;
        for (uint8_t j = 0; j < NDistances; j++) {
            // TODO: second statement is meaningless
            if (Sensor[i].Distances[j] != 0  && Sensor[i].Distances[j] < MIN_DIST) {
                Total += Sensor[i].Distances[j];
                Num += 1;
            }
        }
        if (Num > 3) {
            Sensor[i].MeanDistance = Total / Num;
        } else {
            Sensor[i].MeanDistance = 0;
        }
    }
    
#ifdef DEBUG_SENSORS
    if (millis() - last_time > TIME_DELTA)
    {
        COM_PORT.print("\n\rDistancias: ");
        COM_PORT.print("front = ");
        COM_PORT.print(Sensor[0].MeanDistance);
        COM_PORT.print(",  right = ");
        COM_PORT.print(Sensor[1].MeanDistance);
        COM_PORT.print(", back = ");
        COM_PORT.print(Sensor[2].MeanDistance);
        COM_PORT.print(", left = ");
        COM_PORT.print(Sensor[3].MeanDistance);
        COM_PORT.print(", bottom = ");
        COM_PORT.print(Sensor[4].MeanDistance);
        COM_PORT.print("cm\n\r");

        last_time = millis();
    }
#endif
}



// Check if the mean obtained is below the threshold.
void CheckDistances() {
    // Minimum of 10 for distance. There are measurement errors
    for (uint8_t i = 0; i < NSensors; i++) {
        if (Sensor[i].MeanDistance != 0 && Sensor[i].MeanDistance < ControlDistanceMin) {
            Sensor[i].Close = true;
        } else {
            Sensor[i].Close = false;
        }
    }
}
