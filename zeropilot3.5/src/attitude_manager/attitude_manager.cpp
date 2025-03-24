#include "attitude_manager.hpp"
#include "rc_motor_control.hpp"
#include "direct_mapping.hpp"


bool AttitudeManager::getControlInputs(RCMotorControlMessage_t *pControlMsg) {

    //If no data in the queue
    if (queue_driver->count()==0) {
         return false;
    }
    else {

        *pControlMsg = queue_driver->get();
        queue_driver->pop();
        return true;
    }
}

AttitudeManager::AttitudeManager(Flightmode* controlAlgorithm,  MotorGroupInstance_t rollMotors, MotorGroupInstance_t pitchMotors, MotorGroupInstance_t yawMotors, MotorGroupInstance_t throttleMotors, IMessageQueue<RCMotorControlMessage_t> *queue_driver)
:
    controlAlgorithm_(controlAlgorithm),
    rollMotors_(rollMotors),
    pitchMotors_(pitchMotors),
    yawMotors_(yawMotors),
    throttleMotors_(throttleMotors)
{}   


AttitudeManager::~AttitudeManager()
{}

void AttitudeManager::runControlLoopIteration() {
    // Get data from Queue and motor outputs
    bool res = getControlInputs(&controlMsg);

    if (res != true){
        //No data in the queue
    }

    if (controlMsg.roll == 0 && controlMsg.pitch == 0 && controlMsg.yaw == 0 && controlMsg.throttle == -1) {
        // Do something
    }

    RCMotorControlMessage_t motor_outputs = controlAlgorithm_->run_control(controlMsg); // This is a placeholder for the actual control algorithm

    outputToMotor(yaw, motor_outputs.yaw);
    outputToMotor(pitch, motor_outputs.pitch);
    outputToMotor(roll, motor_outputs.roll);
    outputToMotor(throttle, motor_outputs.throttle);
}



void AttitudeManager::outputToMotor(ControlAxis_e axis, uint8_t percent) {
    MotorGroupInstance_t* motorGroup = nullptr;

    switch (axis) {
        case roll:
            motorGroup = &rollMotors_; //those &Motors is a motor group with multiple motors movements- details undecided
            break;
        case pitch:
            motorGroup = &pitchMotors_;
            break;
        case yaw:
            motorGroup = &yawMotors_;
            break;
        case throttle:
            motorGroup = &throttleMotors_;
            break;
        default:
            return;
    }

    for (uint8_t count = 0; count < motorGroup->motorCount; count++) {
        if (motorGroup->motors[motorCount].isInverted) {
            motorGroup->motors[motorCount].motorInstance->set(100 - percent);
        } else {
            motorGroup->motors[motorCount].motorInstance->set(percent);
        }
    }

}

