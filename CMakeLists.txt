cmake_minimum_required(VERSION 3.28)
project(a1)

set(CMAKE_CXX_STANDARD 20)

add_executable(a1 main.cpp
        clientmain.cpp
        servermain.cpp)
include_directories( ./src/include )