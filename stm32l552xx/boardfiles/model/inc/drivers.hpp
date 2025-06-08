#pragma once

#include "rc.hpp"
#include "gps.hpp"

extern RCReceiver *rcHandle;
extern GPS *gpsHandle;

void initDrivers();
