#include "drivers.hpp"

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart2;

RCReceiver *rcHandle = nullptr;
GPS *gpsHandle = nullptr;

void initDrivers()
{
    rcHandle = new RCReceiver(&huart4); 
    gpsHandle = new GPS(&huart2);
}
