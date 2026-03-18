cmake_minimum_required(VERSION 3.8)
project(practice05)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

add_compile_options(-Wall -Wextra -pedantic)

add_executable(move-semantics move-semantics.cpp)
add_executable(copy-elision copy-elision.cpp)
add_executable(smart-pointers smart-pointers.cpp)
add_executable(aliasing aliasing.cpp)
add_executable(iterator iterator.cpp)
