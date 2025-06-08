#include "drivers.hpp"

extern FDCAN_HandleTypeDef hfdcan1;

void initDrivers()
{
    CAN *canHandle = new CAN(&hfdcan1);
}
