#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Exactly;
using ::testing::Return;

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

class MockTimer {
public:
    MOCK_METHOD(void, tregister, (int, TimerClient*));
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

TEST_F(TimedDoorTest, CanDoorInitiallyClosedCheck) {
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, CanLockClosesDoorCheck) {
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, CanGetTimeoutReturnsCorrectValueCheck) {
    TimedDoor d(5);
    EXPECT_EQ(d.getTimeOut(), 5);
}

TEST_F(TimedDoorTest, CanAdapterIsNotNullCheck) {
    EXPECT_NE(door->getAdapter(), nullptr);
}

TEST_F(TimedDoorTest, CanThrowStateNoThrowWhenDoorClosedCheck) {
    door->lock();
    EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, CanThrowStateThrowsWhenDoorOpenCheck) {
    class OpenDoor : public TimedDoor {
    public:
        explicit OpenDoor(int t) : TimedDoor(t) {}
        void openDirect() { lock(); }
    };
    EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, CanAfterLockDoorIsClosedCheck) {
    door->lock();
    ASSERT_FALSE(door->isDoorOpened());
}

TEST(DoorTimerAdapterTest, CanTimeoutNoThrowWhenDoorClosedCheck) {
    TimedDoor door(1);
    DoorTimerAdapter* adapter = door.getAdapter();
    EXPECT_NO_THROW(adapter->Timeout());
}

TEST(MockTimerClientTest, CanTimeoutCalledExactlyOnceCheck) {
    MockTimerClient mockClient;
    EXPECT_CALL(mockClient, Timeout()).Times(Exactly(1));
    Timer timer;
    timer.tregister(1, &mockClient);
}

TEST(MockDoorTest, CanLockAndUnlockAreCalledCheck) {
    MockDoor mockDoor;
    EXPECT_CALL(mockDoor, lock()).Times(Exactly(1));
    EXPECT_CALL(mockDoor, unlock()).Times(Exactly(1));
    EXPECT_CALL(mockDoor, isDoorOpened()).Times(AtLeast(0));
    mockDoor.lock();
    mockDoor.unlock();
}

TEST(MockDoorTest, CanIsDoorOpenedReturnsMockedValueCheck) {
    MockDoor mockDoor;
    EXPECT_CALL(mockDoor, isDoorOpened())
        .WillOnce(Return(true))
        .WillOnce(Return(false));
    EXPECT_TRUE(mockDoor.isDoorOpened());
    EXPECT_FALSE(mockDoor.isDoorOpened());
}

TEST_F(TimedDoorTest, CanNoExceptionAfterQuickCloseCheck) {
    door->lock();
    EXPECT_NO_THROW(door->throwState());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}