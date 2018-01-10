#include "SerialCommunication.h"    // OpenPorts()
#include "LED.h"                    // LED_init()
#include "functions.h"
#include "variables.h"              // Sensors

// includes of Arduino's libraries. This is for other files
#include <NewPing.h>
#include "mavlink/ardupilotmega/mavlink.h" // Mavlink interface 
#include "mavlink/mavlink_types.h"

#define BAUDRATE 57600

void setup() {
    OpenPorts(BAUDRATE);
    LED_init();
}

void loop() {
    FHeartBeat();
    sensors.MakeMeasureCycle();
    FRCOverride(); // TODO: commented
}
