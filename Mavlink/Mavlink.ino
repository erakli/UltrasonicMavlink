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
    FHeartBeat();    // NB: don't know, why to do this
    sensors.MeasureSensors();
    FRCOverride();

    // delay(1000);
}
