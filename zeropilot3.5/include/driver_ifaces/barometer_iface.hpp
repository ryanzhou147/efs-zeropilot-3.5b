#pragma once

#include "barometer_datatypes.hpp"
#include <cstdint>

class IBarometer {
protected:
    IBarometer() = default;
public:
    virtual ~IBarometer() = default;
    virtual BarometerData_t readData() = 0;
};
