cmake_minimum_required(VERSION 3.8)
project(practice08)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
add_compile_options(-Wall -Wextra -pedantic)

# Basics
add_executable(templates templates.cpp)
add_executable(template-header template-main.cpp template-lib.cpp)
add_executable(template-class template-class.cpp)
add_executable(specialization specialization.cpp)
add_executable(values values.cpp)

# Constraining type arguments
add_executable(type-traits type-traits.cpp)
add_executable(concepts concepts.cpp)
add_executable(template-template template-template.cpp)
