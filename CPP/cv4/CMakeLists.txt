cmake_minimum_required(VERSION 3.8)
project(practice04)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")

add_compile_options(-Wall -Wextra -pedantic)

add_executable(polymorphism polymorphism.cpp)
add_executable(inheritance inheritance.cpp)
add_executable(forward-decl forward-decl.cpp)
add_executable(sum-types sum-types.cpp)
add_executable(type-erasure type-erasure.cpp)
add_executable(interpreter interpreter.cpp)
