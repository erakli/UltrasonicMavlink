#include "sensor.h"

#include <NewPing.h>
#include "defines.h"
#include "pins.h"
#include "variables.h"

#if DEBUG_SENSORS
#include "serial_communication.h"
#endif

/* Initialization of the sensor pins. Library "NewPing"
   NewPing NAME (Trigger, Echo, MAXDIST);
   The value of MAXDIST is the maximum distance the library measures.
   If any Echo returns a value greater than said distance, it is automatically discarded*/

NewPing sonars[] = {
    NewPing(FRONT_TRIGGER,  FRONT_ECHO,  MIN_DIST),  // Front
    NewPing(RIGHT_TRIGGER,  RIGHT_ECHO,  MIN_DIST),  // Right
    NewPing(BACK_TRIGGER,   BACK_ECHO,   MIN_DIST),  // Back
    NewPing(LEFT_TRIGGER,   LEFT_ECHO,   MIN_DIST),  // Left
    NewPing(BOTTOM_TRIGGER, BOTTOM_ECHO, MIN_HEIGHT) // Bottom // NB: here we use MIN_HEIGHT
};

// for non-blocking pings
uint16_t sonars_cm[SONAR_NUM]; // Sonars results
uint8_t currentSensor = 0;         // Keeps track of which sensor is active.



Sensor::Sensor() 
    : meanDistance(0)
    , pingTime(0)
    , isClose(false)
    // , isActive(false)
    // , compensateTime(0) 
{
    for (uint8_t i = 0; i < DISTANCES_NUM; i++) {
        distances[i] = 0;
    }
}


void Sensor::EvalMeanDist() {
    uint32_t total = 0;
    uint8_t num = 0;
    for (uint8_t i = 0; i < DISTANCES_NUM; i++) {
        if (distances[i] != 0) {
            total += distances[i];
            num++;
        }
    }

    if (num > MEAN_MIN_NUM) {
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
    for (uint8_t i = DISTANCES_NUM - 1; i > 0; i--) {
        distances[i] = distances[i - 1];
    }
}



// class Sensors --------------------------------------------------------


Sensors::Sensors()
{
    // First ping starts at 75ms, gives time for the Arduino to chill 
    // before starting.
    sensors_[0].pingTime = millis() + 75;

    // Set the starting time for each sensor.
    for (uint8_t i = 1; i < SONAR_NUM; i++) {
        sensors_[i].pingTime = sensors_[i - 1].pingTime + PING_INTERVAL;
    }
}


// NB: callback-function
// If ping received, set the sensor distance to array.
void echoCheckCallback() { 
  if (sonars[currentSensor].check_timer())
    sonars_cm[currentSensor] = 
        sonars[currentSensor].ping_result / US_ROUNDTRIP_CM;
}


// The sensors are measured, and placed in position 0 of each array
void Sensors::MeasureSensors() {
    for (uint8_t i = 0; i < SONAR_NUM; i++) {
        if (millis() >= sensors_[i].pingTime) {
            sensors_[i].pingTime += PING_INTERVAL * SONAR_NUM;

            if (i == 0 && currentSensor == SONAR_NUM - 1) {
                MeasureCycleFinish();
            }

            // Make sure previous timer is canceled before starting a new ping (insurance).
            sonars[currentSensor].timer_stop(); 
            currentSensor = i;
            sonars_cm[currentSensor] = 0; 
            sonars[currentSensor].ping_timer(echoCheckCallback);
        }
    }
}


void Sensors::MeasureCycleFinish() {
    for (uint8_t i = 0; i < SONAR_NUM; i++) {
        sensors_[i].ShiftDistancesArray();
        sensors_[i].distances[0] = sonars_cm[i];
    }

    EvalMeanDistances();
    CheckDistances();
    Print();
}


// The average of all distances is performed. The 0 are discarded
void Sensors::EvalMeanDistances() {
    for (uint8_t i = 0; i < SONAR_NUM; i++) {
        sensors_[i].EvalMeanDist();
    }
}


// Check if the mean obtained is below the threshold.
void Sensors::CheckDistances() {
    // Minimum of 10 for distance. There are measurement errors
    for (uint8_t i = 0; i < SONAR_NUM; i++) {
        sensors_[i].CheckDistance();
    }
}


void Sensors::Print() {
#if DEBUG_SENSORS
    static unsigned long lastTime = 0;
    if (millis() - lastTime > SENSORS_OUTPUT_TIME)
    {
        for (uint8_t i = 0; i < SONAR_NUM; i++) {
            COM_PORT.print(sensors_[i].meanDistance);
            COM_PORT.print(",");
        }

        COM_PORT.println();
        lastTime = millis();
    }
#endif
}


Sensor& Sensors::operator[](uint8_t idx) {
    return sensors_[idx];
}
