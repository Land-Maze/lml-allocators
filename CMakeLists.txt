cmake_minimum_required(VERSION 3.22)
project(Allocators CXX)

set(CMAKE_CXX_STANDARD 23)

# FIXME: Fix naming and provide better cmake modularity
add_library(LML_Allocators INTERFACE)
target_include_directories(LML_Allocators INTERFACE .)

add_executable(Test main.cpp)
target_link_libraries(Test PRIVATE LML_Allocators)