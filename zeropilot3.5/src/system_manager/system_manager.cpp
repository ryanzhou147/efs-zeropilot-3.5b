#include "system_manager.hpp"

SystemManager::SystemManager(
    IIndependentWatchdog *iwdgDriver,
    ILogger *loggerDriver,
    IRCReceiver *rcDriver,
    IMessageQueue<RCMotorControlMessage_t> *amRCQueue,
    IMessageQueue<char[100]> *smLoggerQueue) :
        iwdgDriver(iwdgDriver),
        loggerDriver(loggerDriver),
        rcDriver(rcDriver),
        amRcQueue(amRCQueue),
        smLoggerQueue(smLoggerQueue) {}

ZP_ERROR_e SystemManager::smUpdate() {
    // Kick the watchdog
    iwdgDriver->refreshWatchdog();

    // Get RC data from the RC receiver and passthrough to AM if new
    static int oldDataCount = 0;
    static bool rcConnected = true;
    
    RCControl rcData;
    
    ZP_ERROR_e rcSuccess = rcDriver->getRCData(rcData);
    if (rcData.isDataNew) {
        oldDataCount = 0;
        ZP_ERROR_e status = sendRCDataToAttitudeManager(rcData);
        
        if (status != ZP_ERROR_OK) return status;

        if (!rcConnected) {
            loggerDriver->log("RC Reconnected");
            rcConnected = true;
        }
    } else {
        oldDataCount += 1;

        if ((oldDataCount * SM_MAIN_DELAY > 500) && rcConnected) {
            loggerDriver->log("RC Disconnected");
            rcConnected = false;
        }
    }

    // Log if new messages
    if (smLoggerQueue->count() > 0) {
        ZP_ERROR_e status = sendMessagesToLogger();
        if (status != ZP_ERROR_OK) return status;
    }

    return rcSuccess;
}

ZP_ERROR_e SystemManager::sendRCDataToAttitudeManager(const RCControl &rcData) {
    RCMotorControlMessage_t rcDataMessage;

    rcDataMessage.roll = rcData.roll;
    rcDataMessage.pitch = rcData.pitch;
    rcDataMessage.yaw = rcData.yaw;
    rcDataMessage.throttle = rcData.throttle;
    rcDataMessage.arm = rcData.arm;
    rcDataMessage.flapAngle = rcData.aux2;

    int statusCode = amRcQueue->push(&rcDataMessage);

    switch (statusCode) {
        case 0: return ZP_ERROR_OK;
        case -1: return ZP_ERROR_FAIL;
        case -2: return ZP_ERROR_TIMEOUT;
        case -3: return ZP_ERROR_RESOURCE_UNAVAILABLE;
        case -4: return ZP_ERROR_INVALID_PARAM;
        case -5: return ZP_ERROR_OUT_OF_MEMORY;
        case -6: return ZP_ERROR_FAIL;
    }

    return ZP_ERROR_FAIL;
}

ZP_ERROR_e SystemManager::sendMessagesToLogger() {
    static char messages[16][100];
    int msgCount = 0;

    while (smLoggerQueue->count() > 0) {
        smLoggerQueue->get(&messages[msgCount]);
        msgCount++;
    }

    loggerDriver->log(messages, msgCount);
    return ZP_ERROR_OK;
}
