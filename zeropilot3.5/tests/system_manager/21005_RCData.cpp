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
    MockRCReceiver mockRC;  // Not using NiceMock here so we can verify strict call expectations
    NiceMock<MockMessageQueue<RCMotorControlMessage_t>> mockAmQueue;
    NiceMock<MockMessageQueue<char[100]>> mockSmLoggerQueue;

    SystemManager sm(&mockIWDG, &mockLogger, &mockRC, &mockAmQueue, &mockSmLoggerQueue);

    mockRC.delegateToFake();

    // Expect getRCData() to be called at least once
    EXPECT_CALL(mockRC, getRCData());

    // Execute the system update
    sm.smUpdate();

}