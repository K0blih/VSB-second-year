cmake_minimum_required(VERSION 3.8)
project(practice06)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
add_compile_options(-Wall -Wextra -pedantic)

add_executable(string string.cpp)
add_executable(string_view string_view.cpp)
add_executable(stream stream.cpp)
add_executable(regex regex.cpp)

add_executable(student_tests student_tests.cpp student.cpp)
