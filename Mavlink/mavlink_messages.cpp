#include "mavlink_messages.h"

#include <ardupilotmega/mavlink.h> // Mavlink interface 
#include <mavlink_types.h>

#include "LED.h"
#include "SerialCommunication.h"



#define SYSTEM_ID        255 // System ID = 255 = GCS
#define COMPONENT_ID     0

void HeartBeat() {
    mavlink_message_t msg;
    uint8_t buf[MAVLINK_MAX_PACKET_LEN];
    uint16_t len;
    
    mavlink_msg_heartbeat_pack(SYSTEM_ID, COMPONENT_ID, &msg, 
        MAV_TYPE_QUADROTOR, MAV_AUTOPILOT_GENERIC, 0, 1, 0);

    // Copy the message to send buffer
    len = mavlink_msg_to_send_buffer(buf, &msg);

    // Send the message (.write sends as bytes)
    APM_PORT.write(buf, len);

#ifdef DEBUG_MAVLINK
    COM_PORT.write("\n\rHeartBeat\n\r");
#endif
}



#define TARGET_SYSTEM    1
#define TARGET_COMPONENT 0
#define CHAN3_RAW        0
#define CHAN4_RAW        0
#define CHAN5_RAW        0
#define CHAN6_RAW        0
#define CHAN7_RAW        0
#define CHAN8_RAW        0

void RCOverride(uint16_t PitchOut, uint16_t RollOut) {
    mavlink_message_t msg;
    uint8_t buf[MAVLINK_MAX_PACKET_LEN];
    uint16_t len;

    // Package and send calculated Pitch and Roll data. Only send if the data is new
    mavlink_msg_rc_channels_override_pack(
        SYSTEM_ID, COMPONENT_ID, &msg, TARGET_SYSTEM, TARGET_COMPONENT, 
        RollOut, PitchOut, CHAN3_RAW, CHAN4_RAW, CHAN5_RAW, CHAN6_RAW, CHAN7_RAW, CHAN8_RAW);
    len = mavlink_msg_to_send_buffer(buf, &msg);
    
    APM_PORT.write(buf, len);

    LED_change_state();

#ifdef DEBUG_MAVLINK
    COM_PORT.print("\n\rPitch: ");
    COM_PORT.print(PitchOut);
    COM_PORT.print(",");
    COM_PORT.print(" Roll: ");
    COM_PORT.print(RollOut);
#endif
}
