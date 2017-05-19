#include "SerialCommunication.h"

void openPorts(unsigned long baudrate) {
    APM_PORT.begin(baudrate);
    COM_PORT.begin(baudrate);
}
