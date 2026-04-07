// Copyright 2022 UNN-CS
#include <gtest/gtest.h>
#include <stdexcept>
#include <functional>
#include "TimedDoor.h"

extern std::function<void(int, TimerClient*)> g_registerTimer;

class TimedDoorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    g_registerTimer = nullptr;
  }
  void TearDown() override {
    g_registerTimer = nullptr;
  }
};

TEST_F(TimedDoorTest, ConstructorSetsTimeoutAndClosed) {
    TimedDoor door(10);
    EXPECT_EQ(door.getTimeOut(), 10);
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOpensDoor) {
    TimedDoor door(5);
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, LockClosesDoor) {
    TimedDoor door(5);
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, TimeoutThrowsWhenDoorOpen) {
    g_registerTimer = [&](int timeout, TimerClient* client) {
        client->Timeout();
    };
    TimedDoor door(1);
    EXPECT_THROW(door.unlock(), std::runtime_error);
}

TEST_F(TimedDoorTest, DoubleUnlockThrowsImmediately) {
    TimedDoor door(5);
    door.unlock();
    EXPECT_THROW(door.unlock(), std::runtime_error);
}

TEST_F(TimedDoorTest, GetTimeOutReturnsCorrectValue) {
    TimedDoor door(15);
    EXPECT_EQ(door.getTimeOut(), 15);
    TimedDoor door2(0);
    EXPECT_EQ(door2.getTimeOut(), 0);
}

TEST_F(TimedDoorTest, DestructorDeletesAdapter) {
    TimedDoor* door = new TimedDoor(3);
    door->unlock();
    delete door;
    EXPECT_TRUE(true);
}

TEST_F(TimedDoorTest, TimerRegisterReceivesCorrectTimeout) {
    int receivedTimeout = -1;
    g_registerTimer = [&](int timeout, TimerClient* client) {
        receivedTimeout = timeout;
    };
    TimedDoor door(7);
    door.unlock();
    EXPECT_EQ(receivedTimeout, 7);
}

TEST_F(TimedDoorTest, TimerRegisterReceivesCorrectClient) {
    TimerClient* registeredClient = nullptr;
    g_registerTimer = [&](int timeout, TimerClient* client) {
        registeredClient = client;
    };
    TimedDoor door(5);
    door.unlock();
    EXPECT_NE(registeredClient, nullptr);
}

TEST_F(TimedDoorTest, ThrowStateThrowsRuntimeError) {
    TimedDoor door(1);
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, AfterExceptionDoorStateRemainsOpen) {
    g_registerTimer = [&](int timeout, TimerClient* client) {
        client->Timeout();
    };
    TimedDoor door(1);
    EXPECT_THROW(door.unlock(), std::runtime_error);
    EXPECT_TRUE(door.isDoorOpened());
}
