#include "serial_communication.h"

#include "pins.h"
#include "constants.h"

#if !defined(__AVR_ATmega2560__)
SoftwareSerial apmSerial(APM_RX, APM_TX);
#endif

void OpenPorts() {
    APM_PORT.begin(APM_BAUDRATE);
    COM_PORT.begin(COM_BAUDRATE);

    COM_PORT.println("Initialized.");
}
