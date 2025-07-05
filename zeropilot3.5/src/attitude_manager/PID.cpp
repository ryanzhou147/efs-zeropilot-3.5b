#include "PID.hpp"
#include "attitude_manager.hpp"
// RCMotorControlMessage_t PID::runControl(RCMotorControlMessage_t controlInputs){
//     // Current State
//     float pitchMeasured = controlInputs.pitch; // What is the band of values this can take? Ask limit of pitch
//     float pitchSetpoint = 50.0f;

//     // Error Calculation
//     float pitchError = pitchSetpoint - pitchMeasured;
//     float pitchDerivative = (pitchError - previousPitchError) / pitchSampleTime;
//     pitchIntegral += pitchError * pitchSampleTime;

//     // PID Computation
//     float pitchControlEffort = (pitch_Kp * pitchError) + (pitch_Kd * pitchDerivative) + (pitch_Ki * pitchIntegral);
//     previousPitchError = pitchError;

//     float pitchOutput = pitchMeasured + pitchControlEffort;

//     // Clamp output. 100 is a placeholder.
//     if (pitchOutput > 100.0f) pitchOutput = 100.0f;
//     if (pitchOutput < 0.0f) pitchOutput = 0.0f;

//     // Set controlInput
//     controlInputs.pitch = pitchOutput;
    
    //starting Phil code here (Template)
    
// Implementation of PID methods (move class definition to header file PID.hpp)

// Constructor
PID::PID(float Kp, float Ki, float Kd,
         float tau, 
         float limMinOutput, float limMaxOutput,
         float limMinInt, float limMaxInt,
         float T) 
    : Kp(Kp), Ki(Ki), Kd(Kd), tau(tau), T(AM_MAIN_DELAY),
      limMinOutput(limMinOutput), limMaxOutput(limMaxOutput),
      limMinInt(limMinInt), limMaxInt(limMaxInt)
{}

// Initialization method
void PID::init() {
    integrator = 0.0f;
    prevError = 0.0f;
    differentiator = 0.0f;
    prevMeasurement = 0.0f;
    out = 0.0f;
}

// Update method
float PID::update(float setpoint, float measurement) {
    // Calculate error
    float error = setpoint - measurement;

    float proportional = Kp * error;

    // Integral 
    integrator = integrator + (0.5f * Ki) * (error + prevError);

    // Anti integral windup
    if (integrator > limMaxInt) integrator = limMaxInt;
    if (integrator < limMinInt) integrator = limMinInt;

    // Calculate differentiator with low-pass filter
    differentiator = (-(2.0f * Kd * (measurement - prevMeasurement)) + ((2 * tau - T) * differentiator)) /((2.0f * tau) + T);
    
    
    // PID output calculation
    out = proportional + integrator + differentiator;
    
    
    // Clamp output
    if (out > limMaxOutput) out = limMaxOutput;
    if (out < limMinOutput) out = limMinOutput;

    // Update previous values
    prevError = error;
    prevMeasurement = measurement;

    return out;
}

