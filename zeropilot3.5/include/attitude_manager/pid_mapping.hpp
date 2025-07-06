#pragma once

#include "flightmode.hpp"
#include "attitude_manager.hpp"
#include "AM_pid.hpp"

class PIDMapping : public Flightmode {
    public:
        PIDMapping() = default;

        RCMotorControlMessage_t runControl(RCMotorControlMessage_t controlInput) override;

        void setRollPIDConstants(float Kp, float Ki, float Kd, float tau) {
            roll_Kp = Kp;
            roll_Ki = Ki;
            roll_Kd = Kd;
            roll_tau = tau;

        }
        void setPitchPIDConstants(float Kp, float Ki, float Kd, float tau) {
            pitch_Kp = Kp;
            pitch_Ki = Ki;
            pitch_Kd = Kd;
            pitch_tau = tau;
        }
    
    private:
        // Roll controls' PID constants
        float roll_Kp = 0.0f;
        float roll_Ki = 0.0f;
        float roll_Kd = 0.0f;
        // Roll integral limits
        const float roll_integral_min_lim = 0.0f;
        float roll_integral_max_lim = 0.0f;
        
        // Pitch controls' PID constants
        float pitch_Kp = 0.0f;
        float pitch_Ki = 0.0f;
        float pitch_Kd = 0.0f;
        // Pitch integral limits
        float pitch_integral_min_lim;
        float pitch_integral_max_lim;

        // Output limits
        const float output_min = 0.0f;
        const float output_max = 0.0f;

        // Tau constants
        float roll_tau = 0.0f;
        float pitch_tau = 0.0f;

        PID rollPID;
        PID pitchPID;
};