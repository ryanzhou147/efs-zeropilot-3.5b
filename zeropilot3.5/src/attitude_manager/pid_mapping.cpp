#include "PID_mapping.hpp"

RCMotorControlMessage_t PIDMapping::runControl(RCMotorControlMessage_t controlInputs){
    // Current State
    float pitchMeasured = controlInputs.pitch; // What is the band of values this can take? Ask limit of pitch
    float pitchSetpoint = 50.0f;

    // Error Calculation
    float pitchError = pitchSetpoint - pitchMeasured;
    float pitchDerivative = (pitchError - previousPitchError) / pitchSampleTime;
    pitchIntegral += pitchError * pitchSampleTime;

    // PID Computation
    float pitchControlEffort = (pitch_Kp * pitchError) + (pitch_Kd * pitchDerivative) + (pitch_Ki * pitchIntegral);
    previousPitchError = pitchError;

    float pitchOutput = pitchMeasured + pitchControlEffort;

    // Clamp output. 100 is a placeholder.
    if (pitchOutput > 100.0f) pitchOutput = 100.0f;
    if (pitchOutput < 0.0f) pitchOutput = 0.0f;

    // Set controlInput
    controlInputs.pitch = pitchOutput;
    return controlInputs;
}