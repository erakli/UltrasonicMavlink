#include "SerialCommunication.h"

void OpenPorts(unsigned long baudrate) {
    APM_PORT.begin(baudrate);
    COM_PORT.begin(baudrate);
}
