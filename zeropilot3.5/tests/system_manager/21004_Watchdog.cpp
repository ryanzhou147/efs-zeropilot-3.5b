#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "iwdg_mock.hpp"

using ::testing::Return;
using ::testing::StrictMock;
using ::testing::_;
using ::testing::InSequence;
using ::testing::NiceMock;

//Test 1: Successful watchdog refresh
TEST(WatchdogTest, RefreshWatchdog_ReturnsTrue){
    StrictMock<MockWatchdog> mockWatchdog;
    EXPECT_CALL(mockWatchdog, refreshWatchdog)
        .WillOnce(Return(true));
    
    // Act and Assert in one step
    EXPECT_TRUE(mockWatchdog.refreshWatchdog());
};

//Test 2: Watchdog refresh failure

TEST(WatchdogTest, RefreshWatchdog_ReturnsFalse){
    StrictMock<MockWatchdog> mockWatchdog;
    EXPECT_CALL(mockWatchdog, refreshWatchdog())
        .WillOnce(Return(false));
    EXPECT_FALSE(mockWatchdog.refreshWatchdog());
}

//Test 3: Multiple calls with different return values

TEST(WatchdogTest, RefreshWatchdog_MultipleTimes){
    StrictMock<MockWatchdog> mockWatchdog;
    EXPECT_CALL(mockWatchdog, refreshWatchdog())
        .WillOnce(Return(true))
        .WillOnce(Return(true))
        .WillOnce(Return(false))
        .WillOnce(Return(true));
    EXPECT_TRUE(mockWatchdog.refreshWatchdog());
    EXPECT_TRUE(mockWatchdog.refreshWatchdog());
    EXPECT_FALSE(mockWatchdog.refreshWatchdog());
    EXPECT_TRUE(mockWatchdog.refreshWatchdog());
}
//Test 4: No expectations set, should return default value (false for bool)
TEST(WatchdogTest, RefreshWatchdog_ReturnDefault){
    NiceMock<MockWatchdog> mockWatchdog;
    EXPECT_FALSE(mockWatchdog.refreshWatchdog());

}
// // Test 5: Verify exact number of calls

TEST(WatchdogTest, RefreshWatchdog_CalledExactNumberOfTimes) {
    
    StrictMock<MockWatchdog> mockWatchdog;
    EXPECT_CALL(mockWatchdog, refreshWatchdog())
        .Times(::testing::Exactly(20))
        .WillRepeatedly(Return(false));

    for (int i = 0; i < 20; ++i) {
        mockWatchdog.refreshWatchdog();
    }

}