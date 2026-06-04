cmake_minimum_required(VERSION 3.14)
project(TimedDoor CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(${PROJECT_SOURCE_DIR}/include)
add_library(timed_door_lib
    src/TimedDoor.cpp
)
target_include_directories(timed_door_lib PUBLIC include)
add_executable(timed_door_app src/main.cpp)
target_link_libraries(timed_door_app timed_door_lib)
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()
add_executable(ST-3.test test/tests.cpp)
target_link_libraries(ST-3.test
    timed_door_lib
    GTest::gtest_main
    GTest::gmock_main
)
include(GoogleTest)
gtest_discover_tests(ST-3.test)
