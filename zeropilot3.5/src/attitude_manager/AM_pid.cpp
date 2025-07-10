#include "AM_pid.hpp"

// Constructor
PID::PID(float kp, float ki, float kd,
         float tau, float outputMinLim, float outputMaxLim,
         float integralMinLim, float integralMaxLim, float t): 
            kp(kp), ki(ki), kd(kd), tau(tau), t(t),
            outputMinLim(outputMinLim), outputMaxLim(outputMaxLim),
            integralMinLim(integralMinLim), integralMaxLim(integralMaxLim)
{}

// Initialization method - Can be used as resetter
void PID::pidInitState() {
    pidIntegral = 0.0f;
    prevError = 0.0f;
    pidDerivative = 0.0f;
    prevMeasurement = 0.0f;
    pidControlEffort = 0.0f;
    pidOutput = 0.0f;
}

// Update method
float PID::pidOutput(float setpoint, float measurement) {
    // Calculate error
    float error = setpoint - measurement;

    // PID Proportional
    float pidProportional = kp * error;

    // PID Integral 
    pidIntegral += (0.5f * ki) * (error + prevError);

    // Anti-integral windup
    if (pidIntegral > integralMaxLim) { pidIntegral = integralMaxLim; }
    if (pidIntegral < integralMinLim) { pidIntegral = integralMinLim; }

    // PID Derivative with low-pass filter
    pidDerivative = ((-1 * 2.0f * kd * (measurement - prevMeasurement)) + ((2 * tau - t) * pidDerivative)) / ((2.0f * tau) + t);
    
    // PID control effort
    pidControlEffort = pidProportional + pidIntegral + pidDerivative;
    
    // Clamp control effort output
    if (pidControlEffort > outputMaxLim) { pidControlEffort = outputMaxLim; }
    if (pidControlEffort < outputMinLim) { pidControlEffort = outputMinLim; }

    // Update previous values
    prevError = error;
    prevMeasurement = measurement;

    // Absolute motor control position
    pidOutput = measurement + pidControlEffort;

    return pidOutput; // Must go directly into motor control
}





