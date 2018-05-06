#ifndef DEFINES_H
#define DEFINES_H

#define DEBUG               false
#define USE_MAVLINK         true
#define DISTANCES_SMOOTHING false
#define USE_PINS_AS_POWER   true

#if DEBUG
    #define LED_INDICATION      false
    #define SOUND_INDICATION    false
    #define DEBUG_SENSORS       false
    #define DEBUG_RC_COMMANDS   false
    #define DEBUG_MOVEMENT      false
    #define DEBUG_MAVLINK       false
#endif

#if DISTANCES_SMOOTHING
    #define RUNNING_AVERAGE             true
    #define EXPONENTIAL_MOVING_AVERAGE  false

    #if RUNNING_AVERAGE && EXPONENTIAL_MOVING_AVERAGE
        #error "Cannot use both filter methods"
    #elif !(RUNNING_AVERAGE || EXPONENTIAL_MOVING_AVERAGE)
        #error "Should be used one of smoothing methods"
    #endif
#endif

#if USE_MAVLINK
    #define HEARTBEAT_MESSAGE true 
    #define ENABLE_RC_CONTROL true
#endif

#endif
