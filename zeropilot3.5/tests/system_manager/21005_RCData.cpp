#include <gtest/gtest.h>
#include "system_manager.hpp"
#include "iwdg_mock.hpp"
#include "logger_mock.hpp"
#include "rc_mock.hpp"
#include "queue_mock.hpp"

using ::testing::_;
using ::testing::NiceMock;

TEST(SMTest, 21005_RCData) 
{  
    NiceMock<MockWatchdog> mockIWDG;
    NiceMock<MockLogger> mockLogger;
    NiceMock<MockRCReceiver> mockRC;
    NiceMock<MockMessageQueue<RCMotorControlMessage_t>> mockAmQueue;
    NiceMock<MockMessageQueue<char[100]>> mockSmLoggerQueue;

    SystemManager sm(&mockIWDG, &mockLogger, &mockRC, &mockAmQueue, &mockSmLoggerQueue);

    mockRC.delegateToFake();

    EXPECT_CALL(mockLogger, log("RC Reconnected"));
    sm.smUpdate();

}