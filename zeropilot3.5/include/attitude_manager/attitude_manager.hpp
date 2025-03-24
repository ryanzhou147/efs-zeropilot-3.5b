#pragma once

#include "flightmode.hpp"
#include "queue_iface.hpp"
#include "motor_iface.hpp"
#include "motor_datatype.hpp"
#include "rc_motor_control.hpp"
#include <stdint.h>


typedef struct {   
    MotorInstance_t* motors;
    uint8_t motorCount;
} MotorGroupInstance_t;

class AttitudeManager {
   public:

    //Are we going to use <T> or a definite variabe type? Using a template here causes imcomplete data type issue in cpp
    AttitudeManager(Flightmode* controlAlgorithm,  MotorGroupInstance_t rollMotors, MotorGroupInstance_t pitchMotors, MotorGroupInstance_t yawMotors, MotorGroupInstance_t throttleMotors, IMessageQueue<RCMotorControlMessage_t> *queue_driver);

    void runControlLoopIteration();

   private:

    RCMotorControlMessage_t controlMsg;
    bool getControlInputs(RCMotorControlMessage_t *pControlMsg);

    void outputToMotor(ControlAxis_e axis, uint8_t percent);
    //What should go into template? AttitudeManagerInput?
    Flightmode*controlAlgorithm_;
    MotorGroupInstance_t rollMotors_;
    MotorGroupInstance_t pitchMotors_;
    MotorGroupInstance_t yawMotors_;
    MotorGroupInstance_t throttleMotors_;
    IMessageQueue<RCMotorControlMessage_t> *queue_driver;
};

    typedef enum {
        yaw,
        pitch,
        roll,
        throttle
    } ControlAxis_e;