#include "functions.h"

#include <Arduino.h>    // millis()
#include <stdint.h>
#include "defines.h"
#include "constants.h"
#include "movement.h"
#include "mavlink_messages.h"
#include "serial_communication.h"

#define RELEASE_CONTROL_TO_RC   0
#define PERSIST_PREVIOUS_VALUE  -1  // UINT_MAX


// It is a State Machine
class ChannelCommand {

    typedef enum MovementStates { 
        MovementState_Still,
        MovementState_NeedMovement,
        MovementState_Movement,
        MovementState_NeedCompensate,
        MovementState_Compensate,
        MovementState_ReleaseControl
    };

public:
    ChannelCommand(Channels channel)
        : channel(channel)
        , state(MovementState_Still)
        , value(0)
        , outValue(0)
        , time(0)
        , sameDesiredManeuverCount(0)
    {
    }

    // main method
    void Eval() {
        DoStopMove();
        UpdateDesiredManeuever();
        if (NeedMovement()) {
            DoMove();
        }
    }

    bool NeedMovement() const {
        return (state != MovementState_Still);
    }

    uint16_t GetValue() const {
        return outValue;
    }

private:
    void ChangeState() {
        switch (state) {
            case MovementState_NeedMovement: 
                state = MovementState_Movement;
            break;

            case MovementState_Movement: 
                // NOTE: currently pass compensation
                // state = MovementState_NeedCompensate;
                state = MovementState_ReleaseControl;
            break;

            case MovementState_NeedCompensate: 
                state = MovementState_Compensate;
            break;

            case MovementState_Compensate: 
                state = MovementState_ReleaseControl;
            break;

            case MovementState_ReleaseControl: 
                state = MovementState_Still;
            break;
        }
    } 

    void UpdateDesiredManeuever() {
        // NOTE: here's serious coupling (bad)
        uint16_t calculated = CheckChannel(channel);

        if (calculated != value) {
            sameDesiredManeuverCount = 0;
            value = calculated;
        } else {
            sameDesiredManeuverCount++;
        }

        if (STABLE_CHANNEL_VALUE_COUNT < sameDesiredManeuverCount)
            state = MovementState_NeedMovement;
    }

    void DoMove() {
        switch (state) {
            case MovementState_NeedMovement:
                StartTime();
                ChangeState();
                outValue = GetRcCommand();
            break;

            case MovementState_NeedCompensate:
                StartTime();
                ChangeState();
                outValue = GetCompensateCommand();
            break;

            case MovementState_ReleaseControl:
                StopTime();
                ChangeState();
                outValue = RELEASE_CONTROL_TO_RC;
            break;

            default:
                // NB: test, if we send PERSIST_PREVIOUS_VALUE when control is
                // currently on RC
                outValue = PERSIST_PREVIOUS_VALUE;
        }
    }

    void DoStopMove() {
        switch (state) {
            case MovementState_Movement:
                if (CheckTime(MOVEMENT_TIME)) {
                    StopTime();
                    ChangeState();
                }
            break;

            case MovementState_Compensate:
                if (CheckTime(COMPENSATE_TIME)) {
                    StopTime();
                    ChangeState();
                }
            break;
        }
    }

    // NOTE: we can add coefficients
    uint16_t GetRcCommand() const {
        return (value != 0) ? ZERO_RC_VALUE + value : RELEASE_CONTROL_TO_RC;
    }

    // NOTE: we can add coefficients
    uint16_t GetCompensateCommand() {
        return (value != 0) ? ZERO_RC_VALUE - value : RELEASE_CONTROL_TO_RC;
    }

    bool CheckTime(uint32_t timeMax) const {
        return (timeMax < millis() - time);
    }

    void StartTime() {
        time = millis();
    }

    void StopTime() {
        time = 0;
    }

private:
    Channels channel;
    MovementStates state;
    uint16_t value;
    uint16_t outValue;
    uint32_t time;
    uint8_t sameDesiredManeuverCount;
};


ChannelCommand desiredPitch(Channel_Pitch);
ChannelCommand desiredRoll(Channel_Roll);


void FHeartBeat() {
#if HEARTBEAT_MESSAGE
    static unsigned long lastTime = 0;

    if (HEARTBEAT_OUTPUT_TIME < millis() - lastTime) {
        HeartBeat();
        lastTime = millis();
    }
#endif
}


void FRCOverride() {
    desiredPitch.Eval();
    desiredRoll.Eval();
    if (desiredPitch.NeedMovement() || desiredRoll.NeedMovement())
        RCOverride(desiredRoll.GetValue(), desiredPitch.GetValue());
}
