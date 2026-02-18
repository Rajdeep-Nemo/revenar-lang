# Build the libraries in order of dependency
add_subdirectory(common)
add_subdirectory(lexer)
add_subdirectory(parser)
add_subdirectory(evaluator)

# Create the final executable
add_executable(lyka main.c)

# Link the executable to the modules
target_link_libraries(lyka PRIVATE
    lyka_evaluator
    lyka_parser
    lyka_lexer
    lyka_common
)
