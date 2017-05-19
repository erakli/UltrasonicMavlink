#ifndef SENSORS_H
#define SENSORS_H

#include <NewPing.h>

// if Arduino IDE is too old we need do some work around
#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 7)
    #define OLD_COMPILER 
#endif

/*  Sensor0 = Front
    Sensor1 = Right
    Sensor2 = Rear
    Sensor3 = Left */

/* Initialization of the sensor pins. Library "NewPing"
   NewPing NAME (Trigger, Echo, MAXDIST);
   The value of MAXDIST is the maximum distance the library measures.
   If any Echo returns a value greater than said distance, it is automatically discarded*/

extern NewPing sonar0;
extern NewPing sonar1;
extern NewPing sonar2;
extern NewPing sonar3;
extern NewPing sonar4;


#define NDistances   5

// Struct for saving the data of each sensor
struct Sensors {
#ifndef OLD_COMPILER
    uint16_t Distances[NDistances]  = {0};
    uint16_t MeanDistance          = 0;
    bool Close                      = false;
    bool Active                     = false;
    unsigned long CompensateTime    = 0;
#else
    uint16_t Distances[NDistances];
    uint16_t MeanDistance;
    bool Close;
    bool Active;
    unsigned long CompensateTime;
    
    Sensors();
#endif
};


// The sensors are measured, and placed in position 0 of each array
void MeasureSensors();

// The average of all distances is performed. The 0 are discarded
void MeanDistances();

// Check if the mean obtained is below the threshold.
void CheckDistances();

#endif
