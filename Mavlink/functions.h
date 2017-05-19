#ifndef FUNCTIONS_H
#define FUNCTIONS_H

// Task to measure the sensors
void FSensors();

// Task responsible for sending a HeartBeat every second
void FHeartBeat();

// Task that sends the motion commands according to the distances detected by the sensors
void FRCOverride();

// Shift each array of Distances on one position to the right
void ShiftArrays();

#endif
