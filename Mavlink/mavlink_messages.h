#ifndef MAVLINK_MESSAGES_H
#define MAVLINK_MESSAGES_H

void HeartBeat();
void RCOverride(uint16_t roll, uint16_t pitch, uint16_t throttle = 0, uint16_t yaw = 0);

#endif
