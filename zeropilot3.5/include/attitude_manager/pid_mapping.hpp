#pragma once

#include "flightmode.hpp"
#include "attitude_manager.hpp"
#include "AM_pid.hpp"

class PIDMapping : public Flightmode {
    public:
        PIDMapping();

        RCMotorControlMessage_t runControl(RCMotorControlMessage_t controlInput) override;

        // Setter *roll* for PID consts + time constant tau for the derivative low-pass filter
        void setRollPIDConstants(float kp, float ki, float kd, float tau) {
            roll_kp = kp;
            roll_ki = ki;
            roll_kd = kd;
            rollTau = tau;

        }

        // Setter for *pitch* PID consts + time constant tau for the derivative low-pass filter
        void setPitchPIDConstants(float kp, float ki, float kd, float tau) {
            pitch_kp = kp;
            pitch_ki = ki;
            pitch_kd = kd;
            pitchTau = tau;
        }
    
    private:
        // Roll controls' PID constants
        float roll_kp = 0.0f;
        float roll_ki = 0.0f;
        float roll_kd = 0.0f;
        // Roll integral limits
        const float ROLL_INTEGRAL_MIN_LIM = 0.0f;
        const float ROLL_INTEGRAL_MAX_LIM = 0.0f;
        
        // Pitch controls' PID constants
        float pitch_kp = 0.0f;
        float pitch_ki = 0.0f;
        float pitch_kd = 0.0f;
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
        PID rollPID;
        PID pitchPID;
};