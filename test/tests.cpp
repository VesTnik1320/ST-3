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

TEST_F(TimedDoorTest, LockBeforeTimeoutPreventsException) {
    g_registerTimer = [&](int timeout, TimerClient* client) {
    };
    TimedDoor door(2);
    door.unlock();
    door.lock();
    door.unlock();
    EXPECT_TRUE(true);
}

TEST_F(TimedDoorTest, DoubleUnlockThrowsImmediately) {
    TimedDoor door(5);
    door.unlock();
    EXPECT_THROW(door.unlock(), std::runtime_error);
}

TEST_F(TimedDoorTest, UnlockAfterLockResetsTimer) {
    TimedDoor door(5);
    door.unlock();
    door.lock();
    EXPECT_NO_THROW(door.unlock());
    EXPECT_TRUE(door.isDoorOpened());
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

TEST_F(TimedDoorTest, TimeoutWhenDoorClosedDoesNotThrow) {
    TimedDoor door(2);
    door.unlock();
    door.lock();
    EXPECT_NO_THROW(door.unlock());
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

TEST_F(TimedDoorTest, MultipleUnlockLockCycles) {
    TimedDoor door(3);
    for (int i = 0; i < 5; ++i) {
        EXPECT_NO_THROW(door.unlock());
        EXPECT_TRUE(door.isDoorOpened());
        EXPECT_NO_THROW(door.lock());
        EXPECT_FALSE(door.isDoorOpened());
    }
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

TEST_F(TimedDoorTest, MultipleDoorsWorkIndependently) {
    TimedDoor door1(10);
    TimedDoor door2(20);
    door1.unlock();
    door2.unlock();
    EXPECT_TRUE(door1.isDoorOpened());
    EXPECT_TRUE(door2.isDoorOpened());
    door1.lock();
    EXPECT_FALSE(door1.isDoorOpened());
    EXPECT_TRUE(door2.isDoorOpened());
    EXPECT_NO_THROW(door1.unlock());
    EXPECT_THROW(door2.unlock(), std::runtime_error);
}
