#include <ardupilotmega/mavlink.h> // Mavlink interface 
#include <mavlink_types.h>

#include <NewPing.h>

// if Arduino IDE is too old we need do some work around
#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 7)
    #define OLD_COMPILER
#endif

#define DEBUG

//==================================INITIALIZATIONS======================================//
/* Initialization of the sensor pins. Library "NewPing"
   NewPing NAME (Trigger, Echo, MAXDIST);
   The value of MAXDIST is the maximum distance the library measures.
   If any Echo returns a value greater than said distance, it is automatically discarded*/
#define MIN_DIST 300
#define MIN_HEIGHT 10      // TODO: changed from 100

NewPing sonar0(4, 5, MIN_DIST);
NewPing sonar1(8, 9, MIN_DIST);
NewPing sonar2(24, 25, MIN_DIST);
NewPing sonar3(27, 28, MIN_DIST);
NewPing sonar4(46, 47, MIN_HEIGHT);

// Variable used to control the HeartBeat sent every second
unsigned long HeartbeatTime = 0;

// Variables used to send only one RCOverride at a time
// to be modified, and not to saturate the redundant order controller
uint16_t Pitch        = 0;
uint16_t Roll         = 0;
uint16_t PitchOut     = 0;
uint16_t RollOut      = 0;
uint16_t PitchOutTemp = 0;
uint16_t RollOutTemp  = 0;
uint8_t n             = 0;

#define NDistances   5
#define ControlDistanceMin 150 // Distance from which control begins to act // TODO: changed from 100
#define AltMin       70  // Height at which control begins
#define DistMin      50  // Minimum difference between two distances of the same axis to move.
#define Compensation 800 // Time of inertia compensation in ms

// Struct for saving the data of each sensor
struct Sensors {
#ifndef OLD_COMPILER
    uint16_t Distances[NDistances]  = {0};
    uint16_t MeanDistance          = 0;
    bool Close                      = false;
    bool Active                     = false;
    unsigned long CompensateTime    = 0;
#else
    uint16_t Distances[NDistances];
    uint16_t MeanDistance;
    bool Close;
    bool Active;
    unsigned long CompensateTime;
    
    Sensors() : 
        MeanDistance(0), 
        Close(false), 
        Active(false), 
        CompensateTime(0) {
        for (uint8_t i = 0; i < NDistances; i++) {
            Distances[i] = 0;
        }
    }
#endif
};

// The variables of each sensor are created
#define NSensors 5
Sensors Sensor[NSensors];

//====================================PROGRAM============================================//

// on Arduino Mega we have several serial ports, so define
// macro to make affinity
#define APM_PORT    Serial2
#define COM_PORT    Serial1

#ifdef DEBUG
int led = 13;
bool state = false;
#endif

#ifdef DEBUG_SENSORS
unsigned long last_time = 0;
#define TIME_DELTA 400
#endif

void setup() {
    APM_PORT.begin(57600);
    COM_PORT.begin(57600);
#ifdef DEBUG
    pinMode(led, OUTPUT);
    digitalWrite(led, LOW);
#endif
}

void loop() {
    if ( (millis() - HeartbeatTime) > 1000 ) {
        HeartbeatTime = millis();
        FHeartBeat();
    }
    FSensors();
    //FRCOverride();
}

//===========================================FUNCTIONS====================================//
// Task to measure the sensors
void FSensors() {
    ShiftArrays();
    MeasureSensors();
    MeanDistances();
    CheckDistances();
}

// Task that sends the motion commands according to the distances detected by the sensors
void FRCOverride() {
    // these variables can be moved inside RCOverride
    mavlink_message_t msg;
    uint8_t buf[MAVLINK_MAX_PACKET_LEN];

    // TODO: temp assignment
    Pitch = 0;
    Roll = 0;
//    Pitch  = CheckPitch(Pitch);
    Roll   = CheckRoll(Roll);     // TODO: temp commented

    CompensationInertia();
 
    if ( Pitch != PitchOutTemp || Roll != RollOutTemp ) {
        n = 0;
        PitchOutTemp = Pitch;
        RollOutTemp  = Roll;
    } else {
        n += 1;
        if (n == 4) {
            RollOut = RollOutTemp;
            PitchOut = PitchOutTemp;
            RCOverride(&msg, buf, PitchOut, RollOut);
        }
    }
}

// Shift each array of Distances on one position to the right
void ShiftArrays() {
    for (uint8_t i = 0; i < NSensors; i++) {
        for (uint8_t j = NDistances - 1; j > 0; j--) {
            Sensor[i].Distances[j] = Sensor[i].Distances[j - 1];
        }
    }
}

//==================================SENSORS=====================================//
// The sensors are measured, and placed in position 0 of each array
void MeasureSensors() {
    Sensor[0].Distances[0] = sonar0.ping_cm();
    Sensor[1].Distances[0] = sonar1.ping_cm();
    Sensor[2].Distances[0] = sonar2.ping_cm();
    Sensor[3].Distances[0] = sonar3.ping_cm();
    Sensor[4].Distances[0] = sonar4.ping_cm();
}

// The average of all distances is performed. The 0 are discarded
void MeanDistances() {
    int Total;
    uint8_t Num;
    for (uint8_t i = 0; i < NSensors; i++) {
        Total = 0;
        Num = 0;
        for (uint8_t j = 0; j < NDistances; j++) {
            // TODO: second statement is meaningless
            if (Sensor[i].Distances[j] != 0  && Sensor[i].Distances[j] < MIN_DIST) {
                Total += Sensor[i].Distances[j];
                Num += 1;
            }
        }
        if (Num > 3) {
            Sensor[i].MeanDistance = Total / Num;
        } else {
            Sensor[i].MeanDistance = 0;
        }
    }
    
#ifdef(DEBUG_SENSORS)
    if (millis() - last_time > TIME_DELTA)
    {
        COM_PORT.print("\n\rDistancias: ");
        COM_PORT.print("front = ");
        COM_PORT.print(Sensor[0].MeanDistance);
        COM_PORT.print(",  right = ");
        COM_PORT.print(Sensor[1].MeanDistance);
        COM_PORT.print(", back = ");
        COM_PORT.print(Sensor[2].MeanDistance);
        COM_PORT.print(", left = ");
        COM_PORT.print(Sensor[3].MeanDistance);
        COM_PORT.print(", bottom = ");
        COM_PORT.print(Sensor[4].MeanDistance);
        COM_PORT.print("cm\n\r");

        last_time = millis();
    }
#endif
}

// Check if the mean obtained is below the threshold.
void CheckDistances() {
    // Minimum of 10 for distance. There are measurement errors
    for (uint8_t i = 0; i < NSensors; i++) {
        if (Sensor[i].MeanDistance != 0 && Sensor[i].MeanDistance < ControlDistanceMin) {
            Sensor[i].Close = true;
        } else {
            Sensor[i].Close = false;
        }
    }
}

//========================MOVEMENT=========================//
// TODO: meaningless argument
uint16_t CheckPitch(uint16_t Pitch) {
    int16_t Difference = Sensor[0].MeanDistance - Sensor[2].MeanDistance;
    if ( Sensor[4].MeanDistance > AltMin || Sensor[4].MeanDistance == 0 ) {
        if ( abs(Difference) > DistMin ) {
            // Difference greater than 30 between both sensors
            if ( Sensor[0].Close == true ) {
                // Detects the front
                if ( Sensor[2].Close == true ) {
                    // Detects the butt
                    if ( Sensor[0].MeanDistance < Sensor[2].MeanDistance ) {
                        // The front sensor has a shorter distance
                        return( Pitch = ValueRC( Sensor[0].MeanDistance, 1 ) );
                    } else {
                        // The rear sensor has a shorter distance
                        return( Pitch = ValueRC( Sensor[2].MeanDistance, 0 ) );
                    }
                } else {
                    // Detects front, but not rear
                    return( Pitch = ValueRC( Sensor[0].MeanDistance, 1 ) );
                }
            } else {
                // Does not detect the front
                if ( Sensor[2].Close == true ) {
                    // Detects the butt
                    return( Pitch = ValueRC( Sensor[2].MeanDistance, 0 ) );
                } else {
                    // Both have a distance greater than 150
                    return( Pitch = 0 );
                }
            }
        } else if ( Sensor[0].Close == true && Sensor[2].MeanDistance == 0 ) {
            // Detect the forward, and the back to detect nothing, returns 0
            return( Pitch = ValueRC( Sensor[0].MeanDistance, 1 ) );
        } else if ( Sensor[0].MeanDistance == 0 && Sensor[2].Close == true ) {
            // The same but the opposite
            return( Pitch = ValueRC( Sensor[2].MeanDistance, 0 ) );
        } else {
            // Does not detect any. Both at 0
            return( Pitch = 0 );
        }
    } else {
        return( Pitch = 0 );
    }
}

// TODO: meaningless argument
uint16_t CheckRoll(uint16_t Roll) {
    int16_t Difference = Sensor[1].MeanDistance - Sensor[3].MeanDistance;
    if ( Sensor[4].MeanDistance > AltMin || Sensor[4].MeanDistance == 0 ) {
        if ( abs(Difference) > DistMin ) {
            // Difference greater than 20 between distances
            if ( Sensor[1].Close == true ) {
                // Detects the right
                if ( Sensor[3].Close == true ) {
                    // Detects left
                    if ( Sensor[1].MeanDistance < Sensor[3].MeanDistance ) {
                        // The right sensor has a smaller distance
                        return( Roll = ValueRC( Sensor[1].MeanDistance, 0 ) );
                    } else {
                        // The left sensor has a smaller distance
                        return( Roll = ValueRC( Sensor[3].MeanDistance, 1 ) );
                    }
                } else {
                    // Detects right, but not left
                    return( Roll = ValueRC( Sensor[1].MeanDistance, 0 ) );
                }
            } else {
                //No detecta el derecho
                if ( Sensor[3].Close == true ) {
                    // Detects left
                    return( Roll = ValueRC( Sensor[3].MeanDistance, 1 ) );
                } else {
                    // Both have a distance greater than 150
                    return( Roll = 0 );
                }
            }
        } else if ( Sensor[1].Close == true && Sensor[3].MeanDistance == 0 ) {
            // Detects the right, and the left when it does not detect anything, returns 0
            return( Roll = ValueRC( Sensor[1].MeanDistance, 0 ) );
        } else if ( Sensor[1].MeanDistance == 0 && Sensor[3].Close == true ) {
            // The same but the opposite
            return( Roll = ValueRC( Sensor[3].MeanDistance, 1 ) );
        } else {
            // Does not detect any. Both at 0
            return( Roll = 0 );
        }
    } else {
        return( Roll = 0 );
    }
}

// Returns an output value depending on the distance
// The greater the distance, the lower the need for movement.
// The variable "Increase" is to know in which direction it is.
uint16_t ValueRC( uint16_t Distance, bool Increase ) {
    if ( Distance < 30 ) {
        if ( Increase == true ) {
            return( 1700 );
        } else {
            return( 1300 );
        }
    } else if ( Distance < 90 ) {
        if ( Increase == true ) {
            return( 1675 );
        } else {
            return( 1325 );
        }
    } else if ( Distance < 150 ) {
        if ( Increase == true ) {
            return( 1650 );
        } else {
            return( 1350 );
        }
    }
}

void CompensationInertia() {

    if (PitchOut > 1500 && Sensor[0].Active == false && Sensor[2].Active == false) {
        Sensor[0].Active = true;
    } else if (PitchOut < 1500 && PitchOut != 0 && Sensor[2].Active == false && Sensor[0].Active == false) {
        Sensor[2].Active = true;
    } else if (PitchOut == 0 && Sensor[0].Active == true && Sensor[0].CompensateTime == 0) {
        Sensor[0].CompensateTime = millis();
    } else if (PitchOut == 0 && Sensor[2].Active == true && Sensor[2].CompensateTime == 0) {
        Sensor[2].CompensateTime = millis();
    }

    if (RollOut > 1500 && Sensor[3].Active == false && Sensor[1].Active == false) {
        Sensor[3].Active = true;
    } else if (RollOut < 1500 && RollOut != 0 && Sensor[1].Active == false && Sensor[3].Active == false) {
        Sensor[1].Active = true;
    } else if (RollOut == 0 && Sensor[1].Active == true && Sensor[1].CompensateTime == 0) {
        Sensor[1].CompensateTime = millis();
    } else if (RollOut == 0 && Sensor[3].Active == true && Sensor[3].CompensateTime == 0) {
        Sensor[3].CompensateTime = millis();
    }

    for (int i = 0; i < 4; i++) {
        if (Sensor[i].CompensateTime != 0 && (Sensor[i].CompensateTime + Compensation > millis())) {
            switch (i) {
            case 0:
                Pitch = 1300;
                break;
            case 1:
                Roll = 1700;
                break;
            case 2:
                Pitch = 1700;
                break;
            case 3:
                Roll = 1300;
                break;
            default:
                break;
            }
        } else if (Sensor[i].CompensateTime != 0) {
            switch (i) {
            case 0:
            case 2:
                PitchOut = 0;
                Sensor[i].Active = false;
                Sensor[i].CompensateTime = 0;
                break;
            case 1:
            case 3:
                RollOut = 0;
                Sensor[i].Active = false;
                Sensor[i].CompensateTime = 0;
                break;
            default:
                break;
            }
        }
    }

}

//============================MAVLINK==========================//
// Task responsible for sending a HeartBeat every second
void FHeartBeat() {
    mavlink_message_t msg;
    uint8_t buf[MAVLINK_MAX_PACKET_LEN];
    uint16_t len;
    // System ID = 255 = GCS
    mavlink_msg_heartbeat_pack(255, 0, &msg, MAV_TYPE_QUADROTOR, MAV_AUTOPILOT_GENERIC, 0, 1, 0);

    // Copy the message to send buffer
    len = mavlink_msg_to_send_buffer(buf, &msg);

    // Send the message (.write sends as bytes)
    APM_PORT.write(buf, len);

#ifdef DEBUG
    COM_PORT.write("\n\rHeartBeat\n\r");
#endif
}


#define SYSTEM_ID        255
#define COMPONENT_ID     0
#define TARGET_SYSTEM    1
#define TARGET_COMPONENT 0
#define CHAN3_RAW        0
#define CHAN4_RAW        0
#define CHAN5_RAW        0
#define CHAN6_RAW        0
#define CHAN7_RAW        0
#define CHAN8_RAW        0

void RCOverride(mavlink_message_t *msg, uint8_t *buf, uint16_t PitchOut, uint16_t RollOut) {
    // Package and send calculated Pitch and Roll data. Only send if the data is new
    mavlink_msg_rc_channels_override_pack(
        SYSTEM_ID, COMPONENT_ID, msg, TARGET_SYSTEM, TARGET_COMPONENT, 
        RollOut, PitchOut, CHAN3_RAW, CHAN4_RAW, CHAN5_RAW, CHAN6_RAW, CHAN7_RAW, CHAN8_RAW);
    uint16_t len = mavlink_msg_to_send_buffer(buf, msg);
    APM_PORT.write(buf, len);

#ifdef DEBUG
        if (state)
        {
          digitalWrite(led, LOW);
          state = false;
        }
        else
        {
          digitalWrite(led, HIGH);
          state = true;
        }
#endif    

#ifdef DEBUG_0
    COM_PORT.print("\n\rPitch: ");
    COM_PORT.print(PitchOut);
    COM_PORT.print(",");
    COM_PORT.print(" Roll: ");
    COM_PORT.print(RollOut);
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

/*Channel 1 = Roll
  Channel 2 = Pitch
  Channel 3 = Throttle
  Channel 4 = Yaw*/

/* Sensor0 = Front
     Sensor1 = Right
     Sensor2 = Rear
     Sensor3 = Left*/

