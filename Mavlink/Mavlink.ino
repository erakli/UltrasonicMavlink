#include "defines.h"
#include "variables.h"               // Sensors, ...
#include "functions.h"
#include "serial_communication.h"    // OpenPorts()
#include "LED.h"                     // LED_init()


void setup() {
    OpenPorts();
    LED_init();
    sensors.Init();
}

void loop() {
    // we need to send heartbeat at least once per second, so the autopilot
    // now that we (as GSC) are alive
    FHeartBeat();
    sensors.MeasureSensors();
    FRCOverride();

    // delay(1000);
}
