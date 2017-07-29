cmake_minimum_required (VERSION 2.8.12)
project(console-calculator)

option (DEBUG "Build debug Build" off)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

if (DEBUG)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
endif()

set (SOURCES calculator.cpp main.cpp)

include_directories (${PROJECT_SOURCE_DIR})

add_executable (console-calculator ${SOURCES})
