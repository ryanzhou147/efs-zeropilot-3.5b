#include "telemetry_manager.hpp"
#define SYSTEM_ID 1             // Suggested System ID by Mavlink
#define COMPONENT_ID 1          // Suggested Component ID by MAVLINK


TelemetryManager::TelemetryManager(
    IRFD *rfdDriver,
    IMessageQueue<TMMessage_t> *tmQueueDriver,
    IMessageQueue<RCMotorControlMessage_t> *amQueueDriver,
    IMessageQueue<mavlink_message_t> *messageBuffer
) :
    rfdDriver(rfdDriver),
    tmQueueDriver(tmQueueDriver),
    amQueueDriver(amQueueDriver),
    messageBuffer(messageBuffer) {
}

TelemetryManager::~TelemetryManager() = default;


void TelemetryManager::processMsgQueue() {
    while (tmQueueDriver->count() > 0) {
        mavlink_message_t mavlink_message = {};
        TMMessage_t tmq_message = {};
        tmQueueDriver->get(&tmq_message);

        switch (tmq_message.DataType) {
            case TMMessage_t::GPOS_DATA: {
                auto GPOSData = tmq_message.tm_message_data.gposData;
                mavlink_msg_global_position_int_pack(SYSTEM_ID, COMPONENT_ID, &mavlink_message,tmq_message.time_boot_ms,
                    GPOSData.lat, GPOSData.lon, GPOSData.alt, GPOSData.relativeAlt, GPOSData.vx, GPOSData.vy, GPOSData.vz, GPOSData.hdg);
                break; }
            case TMMessage_t::RC_DATA: {
                auto RCData = tmq_message.tm_message_data.rcData;
                mavlink_msg_rc_channels_pack(SYSTEM_ID, COMPONENT_ID, &mavlink_message, tmq_message.time_boot_ms, 6,
                    RCData.roll, RCData.pitch, RCData.yaw, RCData.throttle, RCData.arm, RCData.flapAngle,  // Channel arrangement from system manager
                    UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX,  UINT16_MAX,  UINT16_MAX, UINT16_MAX, UINT8_MAX);
                break; }
            case TMMessage_t::BM_DATA: {
                auto BMData = tmq_message.tm_message_data.bmData;
                mavlink_msg_battery_status_pack(SYSTEM_ID, COMPONENT_ID, &mavlink_message, 255, MAV_BATTERY_FUNCTION_UNKNOWN, MAV_BATTERY_TYPE_LIPO,
                 BMData.temperature, BMData.voltages, BMData.currentBattery, BMData.currentConsumed, BMData.energyConsumed, BMData.batteryRemaining,
                 BMData.timeRemaining, BMData.chargeState, {}, 0, 0); }
            default: {}
                //WHOOPS
        }
        messageBuffer->push(&mavlink_message);
    }
}

void TelemetryManager::heartBeatMsg() {
    MAV_MODE_FLAG base_mode = MAV_MODE_FLAG_MANUAL_INPUT_ENABLED; // Ideally these two fields should be managed by something
    MAV_STATE system_status = MAV_STATE_STANDBY;                  // else like system manager

    mavlink_message_t heartbeat_message = {0};

    mavlink_msg_heartbeat_pack(SYSTEM_ID, COMPONENT_ID, &heartbeat_message, MAV_TYPE_QUADROTOR, MAV_AUTOPILOT_INVALID,
                               base_mode, 0, system_status);
    messageBuffer->push(&heartbeat_message);
}

void TelemetryManager::transmit() {
    uint8_t transmitBuffer[MAVLINK_MSG_MAX_SIZE];
    mavlink_message_t msgToTx{};
        while (messageBuffer->count() > 0) {
            messageBuffer->get(&msgToTx);
            const uint8_t MSG_LEN = mavlink_msg_to_send_buffer(transmitBuffer, &msgToTx);
            rfdDriver->transmit(transmitBuffer, MSG_LEN);
        }
}

void TelemetryManager::reconstructMsg() {


    uint8_t rxBuffer[RX_BUFFER_LEN];

    const uint16_t RECEIVED_BYTES = rfdDriver->receive(rxBuffer, sizeof(rxBuffer));

    //Use mavlink_parse_char to process one byte at a time
    for (uint16_t i = 0; i < RECEIVED_BYTES; ++i) {
        if (mavlink_parse_char(0, rxBuffer[i], &message, &status)) {
            handleRxMsg(message);
            message = {};
        }
    }
}

void TelemetryManager::handleRxMsg(const mavlink_message_t &msg) {
    switch (msg.msgid) {
        case MAVLINK_MSG_ID_PARAM_SET:{
            float valueToSet;
            char paramToSet[MAVLINK_MAX_IDENTIFIER_LEN] = {};
            uint8_t valueType;
            valueToSet = mavlink_msg_param_set_get_param_value(&msg);
            valueType = mavlink_msg_param_set_get_param_type(&msg);

            if(paramToSet[0] == 'A'){ //Would prefer to do this using an ENUM LUT but if this is the only param being set its whatever
                RCMotorControlMessage_t armDisarmMsg{};
                armDisarmMsg.arm = valueToSet;
                amQueueDriver->push(&armDisarmMsg);
            }
            mavlink_message_t response = {};
            mavlink_msg_param_value_pack(SYSTEM_ID, COMPONENT_ID, &response, paramToSet, valueToSet, valueType, 1, 0);
            messageBuffer->push(&response);
            break;}
        default:
            break;
    }
}
