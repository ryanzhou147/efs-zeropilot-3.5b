#include "battery_manager.hpp"

BatteryManager::BatteryManager(float BATT_LOW_VOLT, float BATT_LOW_MAH, float BATT_CRITICAL_VOLT, float BATT_CRITICAL_MAH, IBatteryReciever *bDriver, IMessageQueue<RCMotorControlMessage_t> *amQueueDriver, IMessageQueue<RCMotorControlMessage_t> *smQueueDriver, IIndependentWatchdog *iwdg)
    : BATT_LOW_VOLT(BATT_LOW_VOLT), BATT_LOW_MAH(BATT_LOW_MAH), BATT_CRITICAL_VOLT(BATT_CRITICAL_VOLT), BATT_CRITICAL_MAH(BATT_CRITICAL_MAH), bDriver_(bDriver), amQueueDriver_(amQueueDriver), smQueueDriver_(smQueueDriver), iwdg_(iwdg) {}

void BatteryManager::BMUpdate() {
    // Kick the watchdog
    iwdg_->refreshWatchdog();

    // Get BM
    BatteryCntr batteryData = bDriver_->getBMData(); // placeholder until the battery driver struct and the driver and the getting data function is defined
    std::string Battery_Logic_Detection(BatteryCntr &batteryData);
}
std::string Battery_Logic_Detection(BatteryCntr &batteryData){
    // very rudemnetry checks rn now as we don't have enough information about what's the neighbourhood of voltage under while we send the auto land command
    if (batteryData.cVolt <= BATT_LOW_VOLT){

        // send error (umbrella statement will send autoland if very low) 
    }
    if (batteryData.cVolt <= BATT_Critical_VOLT){

        // send autoland 
    }
    if (batteryData.cMah <= BATT_LOW_MAH){
        // send error 
    }
    if (batteryData.cMah <= BATT_Critical_MAH){
        // send autoland
    }

    else {
        // send everything okay
    }

}