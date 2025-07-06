#include "PID.hpp"
#include "attitude_manager.hpp"

// Constructor
PID::PID(float KP, float KI, float KD,
         float tau, 
         float output_min_lim, float output_max_lim,
         float integral_min_lim, float integral_max_lim,
         float T): 
            KP(KP), KI(KI), KD(KD), tau(tau), T(AM_MAIN_DELAY),
            output_min_lim(output_min_lim), output_max_lim(output_max_lim),
            integral_min_lim(integral_min_lim), integral_max_lim(integral_max_lim)
{}

// Initialization method - Can be used as resetter
void PID::PIDInitState() {
    pid_integral = 0.0f;
    prev_error = 0.0f;
    pid_derivative = 0.0f;
    prev_measurement = 0.0f;
    pid_control_effort = 0.0f;
    pid_output = 0.0f;
}

// Update method
float PID::PIDOutput(float setpoint, float measurement) {
    // Calculate error
    float error = setpoint - measurement;

    // PID Proportional
    float pid_proportional = KP * error;

    // PID Integral 
    pid_integral += (0.5f * KI) * (error + prev_error);

    // Anti-integral windup
    if (pid_integral > integral_max_lim) { pid_integral = integral_max_lim; }
    if (pid_integral < integral_min_lim) { pid_integral = integral_min_lim; }

    // PID Derivative with low-pass filter
    pid_derivative = ((-1 * 2.0f * KD * (measurement - prev_measurement)) + ((2 * tau - T) * pid_derivative)) / ((2.0f * tau) + T);
    
    // PID control effort + output calc.
    pid_control_effort = pid_proportional + pid_integral + pid_derivative;
    pid_output = measurement + pid_control_effort;
    
    // Clamp output
    if (out > output_max_lim) { pid_output = output_max_lim; }
    if (out < output_min_lim) { pid_output = output_min_lim; }

    // Update previous values
    prev_error = error;
    prev_measurement = measurement;

    return pid_output; // Must go directly into motor control
}

RCMotorControlMessage_t PIDMapping::runControl(RCMotorControlMessage_t controlInputs){

}



