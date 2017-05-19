#ifndef MAVLINK_MESSAGES_H
#define MAVLINK_MESSAGES_H

#include <Arduino.h>  // uint16_t

// Task responsible for sending a HeartBeat every second
void HeartBeat();

void RCOverride(uint16_t PitchOut, uint16_t RollOut);

#endif
