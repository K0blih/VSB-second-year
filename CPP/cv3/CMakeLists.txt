cmake_minimum_required(VERSION 3.4)
project(practice03)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")

add_compile_options(-Wall -Wextra -pedantic)

add_executable(lambdas lambdas.cpp)
add_executable(regex regex.cpp)
