// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include "TimedDoor.h"

using ::testing::AtLeast;
using ::testing::Exactly;
using ::testing::Return;

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class MockDoor : public Door {
 public:
  MOCK_METHOD(void, lock,         (), (override));
  MOCK_METHOD(void, unlock,       (), (override));
  MOCK_METHOD(bool, isDoorOpened, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
  TimedDoor* door;

  void SetUp() override {
    door = new TimedDoor(1);
  }

  void TearDown() override {
    delete door;
  }
};

TEST_F(TimedDoorTest, DoorInitiallyClosed) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockClosesDoor) {
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, GetTimeoutReturnsCorrectValue) {
  TimedDoor d(5);
  EXPECT_EQ(d.getTimeOut(), 5);
}

TEST_F(TimedDoorTest, AdapterIsNotNull) {
  EXPECT_NE(door->getAdapter(), nullptr);
}

TEST_F(TimedDoorTest, ThrowStateNoThrowWhenDoorClosed) {
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, ThrowStateThrowsWhenOpen) {
  class FakeDoor : public TimedDoor {
   public:
    explicit FakeDoor(int t) : TimedDoor(t) {}
    void openDirect() { lock(); }
  };
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, AfterLockDoorIsClosed) {
  door->lock();
  ASSERT_FALSE(door->isDoorOpened());
}

TEST(DoorTimerAdapterTest, TimeoutNoThrowWhenDoorClosed) {
  TimedDoor door(1);
  DoorTimerAdapter* adapter = door.getAdapter();
  EXPECT_NO_THROW(adapter->Timeout());
}

TEST(MockTimerClientTest, TimeoutCalledExactlyOnce) {
  MockTimerClient mockClient;
  EXPECT_CALL(mockClient, Timeout()).Times(Exactly(1));
  Timer timer;
  timer.tregister(1, &mockClient);
}

TEST(MockDoorTest, LockAndUnlockAreCalled) {
  MockDoor mockDoor;
  EXPECT_CALL(mockDoor, lock()).Times(Exactly(1));
  EXPECT_CALL(mockDoor, unlock()).Times(Exactly(1));
  EXPECT_CALL(mockDoor, isDoorOpened()).Times(AtLeast(0));
  mockDoor.lock();
  mockDoor.unlock();
}

TEST(MockDoorTest, IsDoorOpenedReturnsMockedValue) {
  MockDoor mockDoor;
  EXPECT_CALL(mockDoor, isDoorOpened())
      .WillOnce(Return(true))
      .WillOnce(Return(false));
  EXPECT_TRUE(mockDoor.isDoorOpened());
  EXPECT_FALSE(mockDoor.isDoorOpened());
}

TEST_F(TimedDoorTest, NoExceptionAfterQuickClose) {
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}
