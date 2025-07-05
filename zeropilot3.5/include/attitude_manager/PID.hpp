#pragma once

#include "flightmode.hpp"

class PID: public Flightmode {
    public:
        RCMotorControlMessage_t runControl(RCMotorControlMessage_t controlInput) override;
         
    
        PID(float Kp, float Ki, float Kd,
                  float tau, 
                  float limMin, float limMax,
                  float limMinInt, float limMaxInt,
                  float T);
        void init();
        float update(float setpoint, float measurement);
    private:
        //gains
        float Kp, Ki, Kd;      // Gains
        float tau;             // Derivative low-pass filter constant
        float T;               // Sample time

        //limits
        float limMinOutput, limMaxOutput;  // Output limits
        float limMinInt, limMaxInt; // Integrator limits
        float integrator;
        float prevError;
        float differentiator;
        float prevMeasurement;
        

        // Output variable
        float out;

        //Im gonna use Phils Lab iteration for the PID controller
        
};
