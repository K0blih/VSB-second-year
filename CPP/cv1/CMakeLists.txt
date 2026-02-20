cmake_minimum_required(VERSION 3.8)
project(practice01)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

add_compile_options(-Wall -Wextra -pedantic)

add_executable(basics basics.cpp)
add_executable(modularization modularization.cpp)
add_executable(references references.cpp)
