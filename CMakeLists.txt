cmake_minimum_required(VERSION 3.5)
project(tomo C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS})

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/tree-sitter/include
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-c/src
)

AUX_SOURCE_DIRECTORY(src SRC)

set(TS_C_SOURCES external/tree-sitter-c/src/parser.c)

add_library(tree_sitter STATIC IMPORTED)
set_target_properties(tree_sitter PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/external/tree-sitter/libtree-sitter.a
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/external/tree-sitter/lib/include
)

add_executable(tomo ${SRC} ${TS_C_SOURCES})

target_link_libraries(tomo
    ${CURSES_LIBRARIES}
    tree_sitter
    m
)

