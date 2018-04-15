#include "serial_communication.h"

#include "constants.h"

#if !defined(__AVR_ATmega2560__) && USE_MAVLINK
    #include "pins.h"
    SoftwareSerial apmSerial(APM_RX, APM_TX);
#endif

void OpenPorts() {
#if USE_MAVLINK
    APM_PORT.begin(APM_BAUDRATE);
#endif
    COM_PORT.begin(COM_BAUDRATE);

    COM_PORT.println("Initialized.");
}
