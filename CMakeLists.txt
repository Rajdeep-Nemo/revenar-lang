cmake_minimum_required(VERSION 3.31.6)
project(lyka-lang C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCC)
    add_compile_options(-Wall -Wextra)
endif()

# Jump into the source folder
add_subdirectory(interpreter)

# Define a switch called LYKA_BUILD_TESTS. Default is OFF.
option(LYKA_BUILD_TESTS "Build the testing suite" OFF)

if(LYKA_BUILD_TESTS)
    message(STATUS "Building Tests...")
    enable_testing()
    add_subdirectory(tests)
endif()
