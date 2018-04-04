#include "defines.h"
#include "variables.h"              // Sensors
#include "functions.h"
#include "SerialCommunication.h"    // OpenPorts()
#include "LED.h"                    // LED_init()

void setup() {
    OpenPorts();
    LED_init();
}

void loop() {
    // FHeartBeat();    // NB: don't know, why do this
    sensors.MeasureSensors();

    FRCOverride();

    if (!COM_PORT.available()) {
        FRCOverride();     
    } else {
        int command = COM_PORT.parseInt();
        COM_PORT.println(command);
    }

    // delay(1000);
}
