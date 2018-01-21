#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h> // uint16_t, etc
#include "constants.h"


// Struct for saving the data of each sensor
class Sensor {
public:
    void EvalMeanDist();
    void CheckDistance();
    void ShiftDistancesArray();

    uint16_t distances[DISTANCES_NUM];
    uint16_t meanDistance;
    unsigned long pingTime;
    bool isClose;
    // bool isActive;
    // unsigned long compensateTime;
    
    Sensor();  
};



class Sensors {
public:
    Sensors();

    void MeasureSensors();
    void MeasureCycleFinish();
    void EvalMeanDistances();
    void CheckDistances();

    void Print();

    Sensor& operator[](uint8_t idx);

private:
    Sensor sensors_[SONAR_NUM];
};


#endif
