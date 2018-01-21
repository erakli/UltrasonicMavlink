#include "defines.h"
#include "variables.h"              // Sensors
#include "functions.h"
#include "SerialCommunication.h"    // OpenPorts()
#include "LED.h"                    // LED_init()

// bool inTakeOff = false;

void setup() {
    OpenPorts();
    LED_init();
}

void loop() {
    // FHeartBeat();    // NB: don't know, why do this
    sensors.MeasureSensors();

    // if (inTakeOff) {
    //     TakeOffCheck();
    // }

    // FRCOverride();

    // if (!COM_PORT.available()) {
    //     FRCOverride();     
    // } else {
    //     int command = COM_PORT.parseInt();
    //     COM_PORT.println(command);
    //     switch (command) {
    //         case 5: {
    //             int height = COM_PORT.parseInt();
    //             COM_PORT.println(height);
    //             TakeOffInit(height);
    //             inTakeOff = true;
    //             break;
    //         }

    //         case 0: {
    //             inTakeOff = false;
    //         }
    //     }
    // }

    // delay(1000);
}
