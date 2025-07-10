#pragma once

#include "flightmode.hpp"
#include "attitude_manager.hpp"
#include "pid.hpp"

class PIDMapping : public Flightmode {
    public:
        PIDMapping();

        RCMotorControlMessage_t runControl(RCMotorControlMessage_t controlInput) override;

        // Setter *roll* for PID consts + time constant tau for the derivative low-pass filter
        void setRollPIDConstants(float kp, float ki, float kd, float tau) {
            rollKp = kp;
            rollKi = ki;
            rollKd = kd;
            rollTau = tau;

        }

        // Setter for *pitch* PID consts + time constant tau for the derivative low-pass filter
        void setPitchPIDConstants(float kp, float ki, float kd, float tau) {
            pitchKp = kp;
            pitchKi = ki;
            pitchKd = kd;
            pitchTau = tau;
        }
    
    private:
        // For the PID roll & pitch consts -> may choose these to be optimized real-time dep. on optimization alg. chosen
        // Roll controls' PID constants
        float rollKp = 0.0f;
        float rollKi = 0.0f;
        float rollKd = 0.0f;
        // Roll integral limits
        const float ROLL_INTEGRAL_MIN_LIM = 0.0f;
        const float ROLL_INTEGRAL_MAX_LIM = 0.0f;
        
        // Pitch controls' PID constants
        float pitchKp = 0.0f;
        float pitchKi = 0.0f;
        float pitchKd = 0.0f;
        // Pitch integral limits
        const float PITCH_INTEGRAL_MIN_LIM;
        const float PITCH_INTEGRAL_MAX_LIM;

        // Output limits (for control effort)
        const float OUTPUT_MIN = 0.0f;
        const float OUTPUT_MAX = 0.0f;

        // tau constants
        float rollTau = 0.0f;
        float pitchTau = 0.0f;

        // Roll and Pitch PID class objects
        PID *rollPID;
        PID *pitchPID;
};