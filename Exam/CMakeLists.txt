cmake_minimum_required(VERSION 3.1)
project(exam)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wpedantic -fPIC")
set(CMAKE_CXX_FLAGS "-pthread")
find_package(Threads REQUIRED)
find_package(GTest REQUIRED)

add_executable(exam    re-examination_C.c)
target_link_libraries(exam pthread)

add_executable(gtest_test main.cpp)
TARGET_LINK_LIBRARIES(gtest_test ${GTEST_BOTH_LIBRARIES} gtest pthread)
