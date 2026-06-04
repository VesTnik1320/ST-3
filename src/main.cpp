// Copyright 2021 GHA Test Team
#include <iostream>
#include "TimedDoor.h"

int main() {
    try {
        TimedDoor door(2);
        door.lock();
        door.unlock();
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
