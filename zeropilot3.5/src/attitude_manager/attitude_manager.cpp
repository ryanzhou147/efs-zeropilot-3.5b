#include "attitude_manager.hpp"

AttitudeManager::AttitudeManager(
    IMessageQueue<RCMotorControlMessage_t> *amQueue,
    IMessageQueue<char[100]> *smLoggerQueue,
    Flightmode *controlAlgorithm,
    MotorGroupInstance_t *rollMotors,
    MotorGroupInstance_t *pitchMotors,
    MotorGroupInstance_t *yawMotors,
    MotorGroupInstance_t *throttleMotors,
    MotorGroupInstance_t *flapMotors,
    MotorGroupInstance_t *steeringMotors
) :
    amQueue(amQueue),
    smLoggerQueue(smLoggerQueue),
    controlAlgorithm(controlAlgorithm),
    rollMotors(rollMotors),
    pitchMotors(pitchMotors),
    yawMotors(yawMotors),
    throttleMotors(throttleMotors),
    flapMotors(flapMotors),
    steeringMotors(steeringMotors) {}

void AttitudeManager::runControlLoopIteration() {
    // Get data from Queue and motor outputs
    bool res = getControlInputs(&controlMsg);

    // Failsafe
    static int noDataCount = 0;
    static bool failsafeTriggered = false;

    if (!res) {
        ++noDataCount;

        if (noDataCount * AM_CONTROL_LOOP_DELAY > AM_FAILSAFE_TIMEOUT) {
            outputToMotor(YAW, 50);
            outputToMotor(PITCH, 50);
            outputToMotor(ROLL, 50);
            outputToMotor(THROTTLE, 0);
            outputToMotor(FLAP_ANGLE, 0);
            outputToMotor(STEERING, 50);

            if (!failsafeTriggered) {
              char errorMsg[100] = "Failsafe triggered";
              smLoggerQueue->push(&errorMsg);
              failsafeTriggered = true;
            }
        }

        return;
    } else {
        noDataCount = 0;

        if (failsafeTriggered) {
          char errorMsg[100] = "Motor control restored";
          smLoggerQueue->push(&errorMsg);
          failsafeTriggered = false;
        }
    }

    // Disarm
    if (controlMsg.arm == 0) {
        controlMsg.throttle = 0;
    }

    RCMotorControlMessage_t motorOutputs = controlAlgorithm->runControl(controlMsg);

    outputToMotor(YAW, motorOutputs.yaw);
    outputToMotor(PITCH, motorOutputs.pitch);
    outputToMotor(ROLL, motorOutputs.roll);
    outputToMotor(THROTTLE, motorOutputs.throttle);
    outputToMotor(FLAP_ANGLE, motorOutputs.flapAngle);
    outputToMotor(STEERING, motorOutputs.yaw);
}

bool AttitudeManager::getControlInputs(RCMotorControlMessage_t *pControlMsg) {
    if (amQueue->count() == 0) {
        return false;
    }

    amQueue->get(pControlMsg);
    return true;
}

void AttitudeManager::outputToMotor(ControlAxis_t axis, uint8_t percent) {
    MotorGroupInstance_t *motorGroup = nullptr;

    switch (axis) {
        case ROLL:
            motorGroup = rollMotors;
            break;
        case PITCH:
            motorGroup = pitchMotors;
            break;
        case YAW:
            motorGroup = yawMotors;
            break;
        case THROTTLE:
            motorGroup = throttleMotors;
            break;
        case FLAP_ANGLE:
            motorGroup = flapMotors;
            break;
        case STEERING:
            motorGroup = steeringMotors;
            break;
        default:
            return;
    }

    for (uint8_t i = 0; i < motorGroup->motorCount; i++) {
        MotorInstance_t *motor = (motorGroup->motors + i);

        if (motor->isInverted) {
            motor->motorInstance->set(100 - percent);
        }
        else {
            motor->motorInstance->set(percent);
        }
    }
}
