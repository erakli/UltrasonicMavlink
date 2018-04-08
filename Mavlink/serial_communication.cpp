#include "serial_communication.h"

#include "constants.h"

void OpenPorts() {
    APM_PORT.begin(APM_BAUDRATE);
    COM_PORT.begin(COM_BAUDRATE);

    COM_PORT.println("Initialized.");
}
