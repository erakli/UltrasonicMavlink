#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "defines.h"

// Sensors --------------------------------------------------------------

#define SONAR_NUM 5

#define MAX_SONAR_DISTANCE 250
#define MAX_SONAR_BOTTOM_DISTANCE 50

#define PING_INTERVAL 50    // interval between each sensor ping

#define CONTROL_DISTANCE_MIN  (MAX_SONAR_DISTANCE - 100) // Distance from which control begins to act

#if CONTROL_DISTANCE_MIN <= 0
    #error "CONTROL_DISTANCE_MIN is not usable"
#endif

#if RUNNING_AVERAGE
    #define DISTANCES_NUM 10
    #define MEAN_MIN_NUM 3
#elif EXPONENTIAL_MOVING_AVERAGE
    #define ALPHA 0.5
#endif

// movement -------------------------------------------------------------

#define MIN_HEIGHT                 0  // Height at which control begins
#define MIN_DISTANCES_DIFFERENCE  50  // Minimum difference between two distances of the same axis to move.
// #define COMPENSATION_TIME   800 // Time of inertia compensation in ms

#define ZERO_RC_VALUE       1500 // uS

// MavLink --------------------------------------------------------------

#define SYSTEM_ID        255 // System ID = 255 = GCS
#define COMPONENT_ID     0

#define TARGET_SYSTEM    1
#define TARGET_COMPONENT 0

#define CHAN5_RAW        0
#define CHAN6_RAW        0
#define CHAN7_RAW        0
#define CHAN8_RAW        0

// functions ------------------------------------------------------------

#define STABLE_CHANNEL_VALUE_COUNT 3

// serial_communication --------------------------------------------------

// 9600, 38400, 57600, 115200

#define APM_BAUDRATE 57600
#define COM_BAUDRATE 57600

// DEBUG output ---------------------------------------------------------

#if DEBUG
    #define SENSORS_OUTPUT_TIME 0
    #define RC_COMMANDS_OUTPUT_TIME 100
    #define HEARTBEAT_OUTPUT_TIME 1000
#endif

#endif