#pragma once

#include "flightmode.hpp"

class PIDMapping : public Flightmode {
    public:
        PIDMapping() = default;

        RCMotorControlMessage_t runControl(RCMotorControlMessage_t controlInput) override;
};
