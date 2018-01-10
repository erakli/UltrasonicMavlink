#ifndef FUNCTIONS_H
#define FUNCTIONS_H

// Task responsible for sending a HeartBeat every second
void FHeartBeat();

// Task that sends the motion commands according to the distances 
// detected by the sensors
void FRCOverride();

#endif
