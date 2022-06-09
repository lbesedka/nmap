cmake_minimum_required(VERSION 3.20)
project(nmap)

#set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD 17)

#include_directories(.)

add_executable(nmap main.cpp TCP.cpp UDP.cpp ArgumentsManager.cpp)
