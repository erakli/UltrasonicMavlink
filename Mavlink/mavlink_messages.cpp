#include "mavlink_messages.h"

#include <stdint.h> // uint16_t, etc
#include "mavlink/ardupilotmega/mavlink.h" // Mavlink interface 

#include "defines.h"
#include "constants.h"
#include "LED.h"
#include "SerialCommunication.h"


void HeartBeat() {
    mavlink_message_t msg;
    mavlink_msg_heartbeat_pack(SYSTEM_ID, COMPONENT_ID, &msg, 
        MAV_TYPE_QUADROTOR, MAV_AUTOPILOT_GENERIC, 0, 1, 0);

    // Copy the message to send buffer
    uint8_t buf[MAVLINK_MAX_PACKET_LEN];
    uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);

    // Send the message (.write sends as bytes)
    APM_PORT.write(buf, len);

#ifdef DEBUG_HEARTBEAT
    COM_PORT.write("\n\rHeartBeat\n\r");
#endif
}



/*Channel 1 = Roll
  Channel 2 = Pitch
  Channel 3 = Throttle
  Channel 4 = Yaw*/

void RCOverride(uint16_t roll, uint16_t pitch, uint16_t throttle, uint16_t yaw) {
    // Package and send calculated Pitch and Roll data. Only send if the data is new
    mavlink_message_t msg;
    mavlink_msg_rc_channels_override_pack(
        SYSTEM_ID, COMPONENT_ID, &msg, TARGET_SYSTEM, TARGET_COMPONENT, 
        roll, pitch, throttle, yaw, 
        CHAN5_RAW, CHAN6_RAW, CHAN7_RAW, CHAN8_RAW);

    uint8_t buf[MAVLINK_MAX_PACKET_LEN];
    uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
    
    APM_PORT.write(buf, len);

    LED_change_state();
    
#ifdef DEBUG_MAVLINK
    COM_PORT.print("time: ");
    COM_PORT.print(millis());
    COM_PORT.print(", Pitch: ");
    COM_PORT.print(pitch);
    COM_PORT.print(", Roll: ");
    COM_PORT.print(roll);
    COM_PORT.print(", Throttle: ");
    COM_PORT.print(throttle);
    COM_PORT.print(", Yaw: ");
    COM_PORT.print(yaw);
    COM_PORT.println();
#endif
}



/*
  //Arm the Dron
  //Pack the message
  //uint16_t mavlink_msg_command_long_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,uint8_t target_system, uint8_t target_component, uint16_t command, uint8_t confirmation, float param1, float param2, float param3, float param4, float param5, float param6, float param7)
  mavlink_msg_command_long_pack(255, 0, &msg, 1, 0, MAV_CMD_COMPONENT_ARM_DISARM, 0, 1, 0, 0, 0, 0, 0, 0);

  len = mavlink_msg_to_send_buffer(buf, &msg);

  // Send the message (.write sends as bytes)
  Serial.write(buf, len);
  delay(1000);*/

/*mavlink_msg_rc_channels_override_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
uint8_t target_system, uint8_t target_component, uint16_t chan1_raw, uint16_t chan2_raw, uint16_t chan3_raw,
uint16_t chan4_raw, uint16_t chan5_raw, uint16_t chan6_raw, uint16_t chan7_raw, uint16_t chan8_raw)*/
