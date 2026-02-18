# Create a library for common utilities
add_library(lyka_common
    mem_util.c
    token.h
    types.h
    mem_util.h
)

target_include_directories(lyka_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
