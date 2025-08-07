cmake_minimum_required(VERSION 3.16)
project(tomo C)

set(CMAKE_C_STANDARD 11)
enable_language(CXX)

include_directories(external/tree-sitter/lib/include)

set(TREE_SITTER_CORE
    external/tree-sitter/lib/src/lib.c
)

file(GLOB_RECURSE SRC src/*.c)

add_executable(${PROJECT_NAME} ${SRC} ${TREE_SITTER_CORE})
target_link_libraries(${PROJECT_NAME} dl ncurses)

# Динамические библиотеки для грамматик
add_library(tree-sitter-c SHARED
    external/tree-sitter-c/src/parser.c
    external/tree-sitter-c/src/scanner.c
)
set_target_properties(tree-sitter-c PROPERTIES
    OUTPUT_NAME "c"
    PREFIX ""
)
install(TARGETS tree-sitter-c DESTINATION lib/tomo-parsers)

add_library(tree-sitter-python SHARED
    external/tree-sitter-python/src/parser.c
    external/tree-sitter-python/src/scanner.c
)
set_target_properties(tree-sitter-python PROPERTIES
    OUTPUT_NAME "python"
    PREFIX ""
)
install(TARGETS tree-sitter-python DESTINATION lib/tomo-parsers)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
