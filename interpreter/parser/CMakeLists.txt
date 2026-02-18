add_library(lyka_parser
    parser_utils.c
    parser_utils.h
    ast.h
    expression.c
    statement.c
)

# Link to lexer (which transitively links common)
target_link_libraries(lyka_parser PUBLIC lyka_lexer)

target_include_directories(lyka_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
