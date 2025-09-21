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
                auto GPOSData = tmq_message.tm_message_data.GPOSData_t;
                mavlink_msg_global_position_int_pack(SYSTEM_ID, COMPONENT_ID, &mavlink_message,tmq_message.time_boot_ms,
                    GPOSData.lat, GPOSData.lon, GPOSData.alt, GPOSData.relative_alt, GPOSData.vx, GPOSData.vy, GPOSData.vz, GPOSData.hdg);
                break; }
            case TMMessage_t::RC_DATA: {
                auto RCData = tmq_message.tm_message_data.RCData_t;
                mavlink_msg_rc_channels_pack(SYSTEM_ID, COMPONENT_ID, &mavlink_message, tmq_message.time_boot_ms, 6,
                    RCData.roll, RCData.pitch, RCData.yaw, RCData.throttle, RCData.arm, RCData.flap_angle,  // Channel arrangement from system manager
                    UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX,  UINT16_MAX,  UINT16_MAX, UINT16_MAX, UINT8_MAX);
                break; }
            case TMMessage_t::BM_DATA: {
                auto BMData = tmq_message.tm_message_data.BMData_t;
                mavlink_msg_battery_status_pack(SYSTEM_ID, COMPONENT_ID, &mavlink_message, 255, MAV_BATTERY_FUNCTION_UNKNOWN, MAV_BATTERY_TYPE_LIPO,
                 BMData.temperature, BMData.voltages, BMData.current_battery, BMData.current_consumed, BMData.energy_consumed, BMData.battery_remaining,
                 BMData.time_remaining, BMData.charge_state, {}, 0, 0); }
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
    uint8_t transmit_buffer[MAVLINK_MSG_MAX_SIZE];
    mavlink_message_t msg_to_tx{};
        while (messageBuffer->count() > 0) {
            messageBuffer->get(&msg_to_tx);
            const uint8_t msg_len = mavlink_msg_to_send_buffer(transmit_buffer, &msg_to_tx);
            rfdDriver->transmit(transmit_buffer, msg_len);
        }
}

void TelemetryManager::reconstructMsg() {


    uint8_t rx_buffer[RX_BUFFER_LEN];

    const uint16_t received_bytes = rfdDriver->receive(rx_buffer, sizeof(rx_buffer));

    //Use mavlink_parse_char to process one byte at a time
    for (uint16_t i = 0; i < received_bytes; ++i) {
        if (mavlink_parse_char(0, rx_buffer[i], &message, &status)) {
            handleRxMsg(message);
            message = {};
        }
    }
}

void TelemetryManager::handleRxMsg(const mavlink_message_t &msg) {
    switch (msg.msgid) {
        case MAVLINK_MSG_ID_PARAM_SET:{
            float value_to_set;
            char param_to_set[MAVLINK_MAX_IDENTIFIER_LEN] = {};
            uint8_t value_type;
            value_to_set = mavlink_msg_param_set_get_param_value(&msg);
            value_type = mavlink_msg_param_set_get_param_type(&msg);

            if(param_to_set[0] == 'A'){ //Would prefer to do this using an ENUM LUT but if this is the only param being set its whatever
                RCMotorControlMessage_t arm_disarm_msg{};
                arm_disarm_msg.arm = value_to_set;
                amQueueDriver->push(&arm_disarm_msg);
            }
            mavlink_message_t response = {};
            mavlink_msg_param_value_pack(SYSTEM_ID, COMPONENT_ID, &response, param_to_set, value_to_set, value_type, 1, 0);
            messageBuffer->push(&response);
            break;}
        default:
            break;
    }
}
