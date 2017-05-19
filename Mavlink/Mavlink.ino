#include "SerialCommunication.h"
#include "LED.h"
#include "functions.h"

// includes of Arduino's libraries. This is for other files
#include <NewPing.h>
#include <ardupilotmega/mavlink.h> // Mavlink interface 
#include <mavlink_types.h>

#define BAUDRATE 57600

void setup() {
    openPorts(BAUDRATE);
    LED_init();
}

void loop() {
    FHeartBeat();
    FSensors();
    FRCOverride(); // TODO: commented
}
