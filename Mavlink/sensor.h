#ifndef SENSOR_H
#define SENSOR_H

#include <NewPing.h>

// if Arduino IDE is too old we need do some work around
#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 7)
    #define OLD_COMPILER 
#endif


#define NDistances   5

// Struct for saving the data of each sensor
class Sensor {
public:
#ifndef OLD_COMPILER
    uint16_t distances[NDistances]  = {0};
    uint16_t meanDistance           = 0;
    bool isClose                    = false;
    bool isActive                   = false;
    unsigned long compensateTime    = 0;
#else
    uint16_t distances[NDistances];
    uint16_t meanDistance;
    bool isClose;
    bool isActive;
    unsigned long compensateTime;
    
    Sensor();
#endif

    void EvalMeanDist();
    void CheckDistance();
    void ShiftDistancesArray();
};


// The sensors are measured, and placed in position 0 of each array
void MeasureSensors();

// The average of all distances is performed. The 0 are discarded
void EvalMeanDistances();

// Check if the mean obtained is below the threshold.
void CheckDistances();

#endif
