cmake_minimum_required(VERSION 3.6)
project(pset)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2")

set(SOURCE_FILES main.cpp persistent_set.cpp persistent_set.h)
add_executable(pset ${SOURCE_FILES})