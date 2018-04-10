#ifndef DEFINES_H
#define DEFINES_H

#define DEBUG true
#define USE_MAVLINK false
#define DISTANCES_SMOOTHING false
#define USE_PINS_AS_POWER true

#if DEBUG
    #define LED_INDICATION false
    #define DEBUG_SENSORS true
#endif

#if DISTANCES_SMOOTHING
    #define RUNNING_AVERAGE false
    #define EXPONENTIAL_MOVING_AVERAGE true

    #if RUNNING_AVERAGE && EXPONENTIAL_MOVING_AVERAGE
        #error "Cannot use both filter methods"
    #elif !(RUNNING_AVERAGE || EXPONENTIAL_MOVING_AVERAGE)
        #error "Should be used one of smoothing methods"
    #endif
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
