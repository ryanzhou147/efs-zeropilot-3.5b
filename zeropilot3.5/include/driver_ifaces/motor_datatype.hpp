#pragma once
#include "motor_iface.hpp"


typedef struct {
    IMotorControl *motorInstance; 
    bool isInverted;
} MotorInstance_t;
