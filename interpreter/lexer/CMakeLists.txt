add_library(lyka_lexer
    lexer.c
    lexer.h
)

target_link_libraries(lyka_lexer PUBLIC lyka_common)

target_include_directories(lyka_lexer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
