cmake_minimum_required(VERSION 3.8)
project(practice02)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")

add_compile_options(-Wall -Wextra -pedantic)

add_executable(casts casts.cpp)
add_executable(lambdas lambdas.cpp)
add_executable(enums enums.cpp)
add_executable(packing packing.cpp)
add_executable(structures structures.cpp)
add_executable(initialization initialization.cpp)
add_executable(raii raii.cpp)
add_executable(optional optional.cpp)
add_executable(api_design api_design.cpp)
