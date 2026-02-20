cmake_minimum_required(VERSION 3.31)
project(Lyka_Interpreter C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Use PROJECT_SOURCE_DIR to keep paths absolute but relative to the project root
include_directories(
    ${PROJECT_SOURCE_DIR}/../shared/include
    ${PROJECT_SOURCE_DIR}/../shared/lexer
    ${PROJECT_SOURCE_DIR}/../shared/parser
    src
)

file(GLOB SHARED_SOURCES
    "../shared/lexer/*.c"
    "../shared/parser/*.c"
)

add_executable(lyka src/main.c src/evaluator.c src/environment.c ${SHARED_SOURCES})