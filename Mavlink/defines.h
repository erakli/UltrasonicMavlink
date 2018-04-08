#ifndef DEFINES_H
#define DEFINES_H

#define DEBUG true
#define USE_MAVLINK false
#define DO_SENSOR_SMOOTHING false

#if DEBUG
    #define LED_INDICATION false
    #define DEBUG_SENSORS true
#endif

#if DO_SENSOR_SMOOTHING
    #define 
#endif

#if USE_MAVLINK
    #define ENABLE_RC_CONTROL false

    #if DEBUG
        #define DEBUG_RC_COMMANDS false
        #define DEBUG_MOVEMENT false
        #define DEBUG_MAVLINK false

        #if DEBUG_MAVLINK
            #define HEARTBEAT_MESSAGE false
        #endif
    #endif
#endif

#endif
