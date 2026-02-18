add_library(lyka_evaluator
    eval.c
    eval.h
    environment.c
    environment.h
)

# Link to parser (which transitively links lexer and common)
target_link_libraries(lyka_evaluator PUBLIC lyka_parser)

target_include_directories(lyka_evaluator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
