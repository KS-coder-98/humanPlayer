cmake_minimum_required(VERSION 3.14)
project(humanPlayer1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-lncurses -pthread")

add_executable(humanPlayer1 humanPlayer.cpp)