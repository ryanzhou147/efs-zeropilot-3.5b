#pragma once

#include "flightmode.hpp"

class PIDMapping : public Flightmode {
    public:
        PIDMapping() = default;

        RCMotorControlMessage_t runControl(RCMotorControlMessage_t controlInput) override;
    private:
        // Pitch controls' PID constants
        float pitch_Kp = 0.0f;
        static float pitch_Ki = 0.0f;
        float pitch_Kd = 0.0f;

        // PID state variables
        float previousPitchError = 0.0f;
        float pitchIntegral = 0.0f;
        float pitchSampleTime = 0.001f;  // Sample time in seconds (100ms)
};
