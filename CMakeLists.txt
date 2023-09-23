cmake_minimum_required(VERSION 3.15)

project(
        lab10
        VERSION 1.0
        DESCRIPTION "C++ Year 2023 Lab10"
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(lib)
add_subdirectory(bin)
add_subdirectory(cpr)
add_subdirectory(json)

include_directories(${CPR_INCLUDE_DIRS})

