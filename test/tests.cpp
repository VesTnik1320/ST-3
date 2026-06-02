// Copyright 2022 UNN-CS
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"

using ::testing::_;
using ::testing::Return;
using ::testing::AtLeast;
using ::testing::Exactly;

class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class MockDoor : public Door {
 public:
    MOCK_METHOD(void, lock, (), (override));
    MOCK_METHOD(void, unlock, (), (override));
    MOCK_METHOD(bool, isDoorOpened, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
    TimedDoor* door;
    void SetUp() override {
        door = new TimedDoor(5);
    }
    void TearDown() override {
        delete door;
    }
};

TEST_F(TimedDoorTest, CanConstructWithTimeoutAndClosedDoor) {
    EXPECT_EQ(door->getTimeOut(), 5);
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, CanUnlockDoor) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, CanLockDoor) {
    door->unlock();
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, CheckDoubleUnlockThrowsException) {
    door->unlock();
    EXPECT_THROW(door->unlock(), std::runtime_error);
}

TEST_F(TimedDoorTest, CanGetCorrectTimeoutValue) {
    EXPECT_EQ(door->getTimeOut(), 5);
    TimedDoor d2(10);
    EXPECT_EQ(d2.getTimeOut(), 10);
}

TEST_F(TimedDoorTest, CheckThrowStateThrowsWhenDoorOpen) {
    door->unlock();
    EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, CheckThrowStateDoesNotThrowWhenDoorClosed) {
    door->lock();
    EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, CanRemainOpenAfterException) {
    door->unlock();
    try {
        door->unlock();
    } catch (...) {}
    EXPECT_TRUE(door->isDoorOpened());
}

class DoorTimerAdapterTest : public ::testing::Test {
 protected:
    TimedDoor* door;
    DoorTimerAdapter* adapter;
    void SetUp() override {
        door = new TimedDoor(3);
        adapter = new DoorTimerAdapter(*door);
    }
    void TearDown() override {
        delete adapter;
        delete door;
    }
};

TEST_F(DoorTimerAdapterTest, CheckTimeoutThrowsWhenDoorOpen) {
    door->unlock();
    EXPECT_THROW(adapter->Timeout(), std::runtime_error);
}

TEST_F(DoorTimerAdapterTest, CheckTimeoutDoesNotThrowWhenDoorClosed) {
    door->lock();
    EXPECT_NO_THROW(adapter->Timeout());
}

TEST(MockDoorTest, CanMockLockMethod) {
    MockDoor mock;
    EXPECT_CALL(mock, lock()).Times(Exactly(1));
    mock.lock();
}

TEST(MockDoorTest, CanMockUnlockMethod) {
    MockDoor mock;
    EXPECT_CALL(mock, unlock()).Times(Exactly(1));
    mock.unlock();
}

TEST(MockDoorTest, CanMockIsDoorOpenedToReturnTrue) {
    MockDoor mock;
    EXPECT_CALL(mock, isDoorOpened()).WillOnce(Return(true));
    EXPECT_TRUE(mock.isDoorOpened());
}

TEST(MockTimerClientTest, CanMockTimeoutMethod) {
    MockTimerClient client;
    EXPECT_CALL(client, Timeout()).Times(AtLeast(1));
    client.Timeout();
}

TEST(TimerTest, ShouldCallClientImmediatelyWhenTimeoutZero) {
    MockTimerClient client;
    Timer timer;
    EXPECT_CALL(client, Timeout()).Times(Exactly(1));
    timer.tregister(0, &client);
}
