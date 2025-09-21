#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "system_manager.hpp"
#include "iwdg_mock.hpp"
#include "logger_mock.hpp"
#include "rc_mock.hpp"
#include "queue_mock.hpp"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::StrictMock;

TEST(SMTest, 21004_Watchdog) 
{  
    StrictMock<MockWatchdog> mockIWDG;
    NiceMock<MockLogger> mockLogger;
    NiceMock<MockRCReceiver> mockRC;
    NiceMock<MockMessageQueue<RCMotorControlMessage_t>> mockAmQueue;
    NiceMock<MockMessageQueue<char[100]>> mockSmLoggerQueue;

    SystemManager sm(&mockIWDG, &mockLogger, &mockRC, &mockAmQueue, &mockSmLoggerQueue);

    // Test 1: Verify watchdog is refreshed on every smUpdate() call
    EXPECT_CALL(mockIWDG, refreshWatchdog());

    sm.smUpdate();

    // Test 2: Verify watchdog is called multiple times across multiple updates
    EXPECT_CALL(mockIWDG, refreshWatchdog());
    EXPECT_CALL(mockIWDG, refreshWatchdog());
    EXPECT_CALL(mockIWDG, refreshWatchdog());

    sm.smUpdate();
    sm.smUpdate();
    sm.smUpdate();
}
