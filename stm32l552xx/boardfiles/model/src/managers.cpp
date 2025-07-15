#include "direct_mapping.hpp"
#include "drivers.hpp"
#include "managers.hpp"

DirectMapping *flightMode = nullptr;
AttitudeManager *amHandle = nullptr;

Logger *loggerHandle = nullptr;
SystemManager *smHandle = nullptr;



void initManagers()
{
    // AM initialization

    flightMode = new DirectMapping();
    amHandle = new AttitudeManager(amRCQueueHandle, smLoggerQueueHandle, flightMode, &rollMotors, &pitchMotors, &yawMotors, &throttleMotors, &flapMotors, &steeringMotors);

    // SM initialization
    loggerHandle = new Logger(textIOHandle);
    smHandle = new SystemManager(iwdgHandle, rcHandle, amRCQueueHandle, smLoggerQueueHandle, loggerHandle);
}
