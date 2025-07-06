#pragma once

#include "flightmode.hpp"

class PIDMapping : public Flightmode {
    public:
        PIDMapping() = default;

        RCMotorControlMessage_t runControl(RCMotorControlMessage_t controlInput) override;
    private:
        // Roll controls' PID constants
        float roll_Kp = 0.0f;
        static float roll_Ki = 0.0f;
        float roll_Kd = 0.0f;

        // Pitch controls' PID constants
        float pitch_Kp = 0.0f;
        static float pitch_Ki = 0.0f;
        float pitch_Kd = 0.0f;
};