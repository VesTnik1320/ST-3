// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <unistd.h>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {}

void DoorTimerAdapter::Timeout() {
  door.throwState();
}

TimedDoor::TimedDoor(int timeout)
    : iTimeout(timeout), isOpened(false) {
  adapter = new DoorTimerAdapter(*this);
}

bool TimedDoor::isDoorOpened() {
  return isOpened;
}

void TimedDoor::unlock() {
  isOpened = true;
  Timer timer;
  timer.tregister(iTimeout, adapter);
}

void TimedDoor::lock() {
  isOpened = false;
}

int TimedDoor::getTimeOut() {
  return iTimeout;
}

void TimedDoor::throwState() {
  if (isOpened) {
    throw std::runtime_error("Door has been open too long!");
  }
}

DoorTimerAdapter* TimedDoor::getAdapter() {
  return adapter;
}

void Timer::sleep(int seconds) {
  ::sleep(static_cast<unsigned int>(seconds));
}

void Timer::tregister(int timeout, TimerClient* c) {
  client = c;
  sleep(timeout);
  client->Timeout();
}
