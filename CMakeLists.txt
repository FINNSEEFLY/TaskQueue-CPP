cmake_minimum_required(VERSION 3.17)
project(TaskQueue)

set(CMAKE_CXX_STANDARD 20)

add_executable(TaskQueue main.cpp TaskQueue.cpp TaskQueue.h)