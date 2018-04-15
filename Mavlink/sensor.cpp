#include "sensor.h"

#include <NewPing.h>
#include "pins.h"

#if DEBUG_SENSORS
#include "serial_communication.h"
#endif

/* Initialization of the sensor pins. Library "NewPing"
   NewPing NAME (Trigger, Echo, MAXDIST);
   The value of MAXDIST is the maximum distance the library measures.
   If any Echo returns a value greater than said distance, it is automatically discarded*/

NewPing sonars[] = {
    NewPing(FRONT_TRIGGER,  FRONT_ECHO,  MAX_SONAR_DISTANCE),       // Front
    NewPing(RIGHT_TRIGGER,  RIGHT_ECHO,  MAX_SONAR_DISTANCE),       // Right
    NewPing(BACK_TRIGGER,   BACK_ECHO,   MAX_SONAR_DISTANCE),       // Back
    NewPing(LEFT_TRIGGER,   LEFT_ECHO,   MAX_SONAR_DISTANCE),       // Left
    NewPing(BOTTOM_TRIGGER, BOTTOM_ECHO, MAX_SONAR_BOTTOM_DISTANCE) // Bottom
};

// for non-blocking pings
uint16_t sonars_cm[SONAR_NUM];      // Sonars results
uint8_t currentSensor = 0;          // Keeps track of which sensor is active.



Sensor::Sensor() 
    : meanDistance(0)
    , pingTime(0)
    , isClose(false)
    // , isActive(false)
    // , compensateTime(0) 
{
#if RUNNING_AVERAGE
    for (uint8_t i = 0; i < DISTANCES_NUM; i++) {
        distances[i] = 0;
    }
#elif EXPONENTIAL_MOVING_AVERAGE
    distance = 0;
#endif
}


void Sensor::Init(uint8_t powerPin, uint8_t groundPin) {
    pinMode(groundPin, OUTPUT);
    pinMode(powerPin, OUTPUT);
    digitalWrite(groundPin, LOW);
    digitalWrite(powerPin, HIGH); 
}


void Sensor::EvalMeanDist() {
#if RUNNING_AVERAGE
    uint32_t total = 0;
    uint8_t num = 0;
    for (uint8_t i = 0; i < DISTANCES_NUM; i++) {
        if (distances[i] != 0) {
            total += distances[i];
            num++;
        }
    }

    if (MEAN_MIN_NUM < num) {
        meanDistance = total / num;
    } else {
        meanDistance = 0;
    }
#elif EXPONENTIAL_MOVING_AVERAGE
    meanDistance = ALPHA * distance + (1.0 - ALPHA) * meanDistance;
#endif
}


void Sensor::CheckDistance() {
    isClose = (meanDistance != 0 && meanDistance < CONTROL_DISTANCE_MIN);
}


#if RUNNING_AVERAGE
// Shift each array of distances on one position to the right
void Sensor::ShiftDistancesArray() {
    for (uint8_t i = DISTANCES_NUM - 1; i > 0; i--) {
        distances[i] = distances[i - 1];
    }
}
#endif



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


void Sensors::Init() {
#if USE_PINS_AS_POWER
    uint8_t powerPins[] = {
        FRONT_POWER, RIGHT_POWER, BACK_POWER, LEFT_POWER};

    uint8_t groundPins[] = {
        FRONT_GROUND, RIGHT_GROUND, BACK_GROUND, LEFT_GROUND};

    for (uint8_t i = 0; i < SONAR_NUM - 1; i++) {
        sensors_[i].Init(powerPins[i], groundPins[i]);
    }
#endif
}


// If ping received, set the sensor distance to array.
void echoCheckCallback() { 
  if (sonars[currentSensor].check_timer()) {
    sonars_cm[currentSensor] = 
        sonars[currentSensor].ping_result / US_ROUNDTRIP_CM;
  }
}


// The sensors are measured, and placed in position 0 of each array
void Sensors::MeasureSensors() {
    for (uint8_t i = 0; i < SONAR_NUM; i++) {
        if (sensors_[i].pingTime <= millis()) {
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
#if RUNNING_AVERAGE
        sensors_[i].ShiftDistancesArray();
        sensors_[i].distances[0] = sonars_cm[i];
#elif EXPONENTIAL_MOVING_AVERAGE
        sensors_[i].distance = sonars_cm[i];
#else
        sensors_[i].meanDistance = sonars_cm[i];
#endif
    }

    EvalMeanDistances();
    CheckDistances();
    Print();
}


// The average of all distances is performed. The 0 are discarded
void Sensors::EvalMeanDistances() {
#if DISTANCES_SMOOTHING
    for (uint8_t i = 0; i < SONAR_NUM; i++) {
        sensors_[i].EvalMeanDist();
    }
#endif
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
    if (SENSORS_OUTPUT_TIME < millis() - lastTime)
    {
        lastTime = millis();
        COM_PORT.print(lastTime);
        COM_PORT.print(",");

        for (uint8_t i = 0; i < SONAR_NUM; i++) {
            COM_PORT.print(sensors_[i].meanDistance);
            COM_PORT.print(",");
        }

        COM_PORT.println();
    }
#endif
}


Sensor& Sensors::operator[](uint8_t idx) {
    return sensors_[idx];
}
