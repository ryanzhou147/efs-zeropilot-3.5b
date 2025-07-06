#pragma once


class PID {
    public:
        // RCMotorControlMessage_t runControl(RCMotorControlMessage_t controlInput) override;
         
        // PID object constructor
        PID(float KP, float KI, float KD, 
            float tau, float output_min_lim, float output_max_lim,
            float integral_min_lim, float integral_max_lim, float T);
        
        // PID object's state var initialized (or reset)
        void PIDInitState();
        
        // 
        float PIDOutput(float setpoint, float measurement);

        ~ PID();

    private:
        // Gains
        float KP, KI, KD;      // PID constants - May choose these to be optimized real-time dep. on optimization alg. chosen
        float tau;             // Derivative low-pass filter constant
        float T;               // Sample time (set to AM_MAIN_DELAY)

        // Output and Integral Limits
        float output_min_lim, output_max_lim;       // Output limits
        float integral_min_lim, integral_max_lim;   // integral limits

        // State variables
        float pid_derivative, pid_integral;
        float prev_error, prev_measurement;

        // Control effort var - redundancy added for clarity
        float pid_control_effort;
        
        // Output var
        float pid_output; // Directly into motor control
};
