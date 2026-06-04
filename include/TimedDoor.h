// Copyright 2021 GHA Test Team
#pragma once
#include <stdexcept>

class TimerClient {
 public:
  virtual void Timeout() = 0;
  virtual ~TimerClient() = default;
};

class Door {
 public:
  virtual void lock() = 0;
  virtual void unlock() = 0;
  virtual bool isDoorOpened() = 0;
  virtual ~Door() = default;
};

class TimedDoor;

class DoorTimerAdapter : public TimerClient {
 private:
  TimedDoor& door;
 public:
  explicit DoorTimerAdapter(TimedDoor&);
  void Timeout() override;
};

class TimedDoor : public Door {
 private:
  DoorTimerAdapter* adapter;
  int iTimeout;
  bool isOpened;
 public:
  explicit TimedDoor(int timeout);
  bool isDoorOpened() override;
  void unlock() override;
  void lock() override;
  int getTimeOut();
  void throwState();
  DoorTimerAdapter* getAdapter();
};

class Timer {
  TimerClient* client;
  void sleep(int seconds);
 public:
  void tregister(int timeout, TimerClient* c);
};
