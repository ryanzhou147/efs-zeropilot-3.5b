#include "drivers.hpp"

extern FDCAN_HandleTypeDef hfdcan1;

CAN *canHandle = nullptr;

void initDrivers()
{
    canHandle = new CAN(&hfdcan1);
}
