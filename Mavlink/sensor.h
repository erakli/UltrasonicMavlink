#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h> // uint16_t, etc

// if Arduino IDE is too old we need do some work around
#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 7)
    #define OLD_COMPILER 
#endif


#define NDistances   5

// Struct for saving the data of each sensor
class Sensor {
public:
    void EvalMeanDist();
    void CheckDistance();
    void ShiftDistancesArray();

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
};



// The variables of each sensor are created
#define NSensors 5

class Sensors {
public:
    void MakeMeasureCycle();

    void MeasureSensors();
    void EvalMeanDistances();
    void CheckDistances();

    void Print();

    Sensor& operator[](uint8_t idx);

private:
    Sensor sensors_[NSensors];
};


#endif
