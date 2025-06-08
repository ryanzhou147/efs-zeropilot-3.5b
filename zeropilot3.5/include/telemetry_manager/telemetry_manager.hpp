#pragma once
#define MAVLINK_MSG_MAX_SIZE 280
#define MALINK_MAX_IDENTIFIER_LEN 17
#define Rx_Buffer_Len 8192

#include <mavlink2/common/mavlink.h>
#include "queue_iface.hpp"
#include "tm_queue.hpp"
#include "rc_motor_control.hpp"
#include "rfd_iface.hpp"
class TelemetryManager {
  private:
    IMessageQueue<mavlink_message_t> *messageBuffer_{};            // GPOS, Attitude and Heartbeat/Connection Messages
    IMessageQueue<TMMessage_t> *tmQueueDriver_;                  // Driver that receives messages from other managers
    IMessageQueue<RCMotorControlMessage_t> *amQueueDriver_;          // Driver that currently is only used to set arm/disarm
    IRFD *rfdDriver_;                                         // Driver used to actually send mavlink messages
    mavlink_status_t status_;
    mavlink_message_t message_;
    void handleRxMsg(const mavlink_message_t &msg);


  public:
    TelemetryManager(IRFD *rfdDriver, IMessageQueue<TMMessage_t>  *tmQueueDriver,  IMessageQueue<TMMessage_t> *amQueueDriver,IMessageQueue<mavlink_message_t> *messageBuffer);
    ~TelemetryManager();
    void processMsgQueue();
    void heartBeatMsg();
    void transmit();
    void reconstructMsg();
};