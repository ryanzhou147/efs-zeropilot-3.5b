#pragma once
class PIDController {
public:
    PIDController(float kp, float ki, float kd, float tau, float T,
                  float limMin, float limMax,
                  float limMinInt, float limMaxInt);

    void init();
    float update(float setpoint, float measurement, float feedforward = 0.0f);

private:
    float Kp, Ki, Kd;      // Gains
    float tau;             // Derivative low-pass filter constant
    float T;               // Sample time
    float limMinOutput, limMaxOutput;  // Output limits
    float limMinInt, limMaxInt; // Integrator limits

    float integrator;
    float prevError;
    float differentiator;
    float prevMeasurement;
    float out;
};