#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h> // uint16_t, etc
#include "defines.h"
#include "constants.h"


// Struct for saving the data of each sensor
class Sensor {
public:
    Sensor();

    void Init(uint8_t powerPin, uint8_t groundPin);

    void EvalMeanDist();
    void CheckDistance();

#if RUNNING_AVERAGE
    void ShiftDistancesArray();
    uint16_t distances[DISTANCES_NUM];
#elif EXPONENTIAL_MOVING_AVERAGE
    uint16_t distance;
#endif
    uint16_t meanDistance;
    unsigned long pingTime;
    bool isClose;
    // bool isActive;
    // unsigned long compensateTime;
};



class Sensors {
public:
    Sensors();

    void Init();

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
