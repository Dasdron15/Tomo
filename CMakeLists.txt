cmake_minimum_required(VERSION 3.16)
project(tomo C)
set(CMAKE_C_STANDARD 11)

file(GLOB_RECURSE SRC src/*.c)

include(FetchContent)
include(GNUInstallDirs)

FetchContent_Declare(
  tree_sitter
  GIT_REPOSITORY https://github.com/tree-sitter/tree-sitter.git
  GIT_TAG master
)
FetchContent_GetProperties(tree_sitter)
if(NOT tree_sitter_POPULATED)
  FetchContent_MakeAvailable(tree_sitter)
endif()

add_library(tree_sitter STATIC ${tree_sitter_SOURCE_DIR}/lib/src/lib.c)
target_include_directories(tree_sitter PUBLIC ${tree_sitter_SOURCE_DIR}/lib/include)
set_target_properties(tree_sitter PROPERTIES POSITION_INDEPENDENT_CODE ON)

function(add_ts_grammar GRAMMAR_NAME GIT_URL)
  set(repo_name ${GRAMMAR_NAME}_repo)
  FetchContent_Declare(${repo_name} GIT_REPOSITORY ${GIT_URL})
  FetchContent_GetProperties(${repo_name})
  if(NOT ${repo_name}_POPULATED)
    FetchContent_Populate(${repo_name})
  endif()
  set(src_dir ${${repo_name}_SOURCE_DIR})
  set(parser_files ${src_dir}/src/parser.c)
  if(EXISTS "${src_dir}/src/scanner.c")
    list(APPEND parser_files ${src_dir}/src/scanner.c)
  endif()
  add_library(ts_${GRAMMAR_NAME} STATIC ${parser_files})
  target_include_directories(ts_${GRAMMAR_NAME} PRIVATE ${src_dir}/src ${tree_sitter_SOURCE_DIR}/lib/include)
  target_link_libraries(ts_${GRAMMAR_NAME} PRIVATE tree_sitter)
  install(TARGETS ts_${GRAMMAR_NAME} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/tomo-parsers)
endfunction()

add_executable(tomo ${SRC})
target_include_directories(tomo PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(tomo PRIVATE tree_sitter ts_c ts_py) # For every new grammar add ts_<language_name>

find_package(Curses REQUIRED)
target_include_directories(tomo PRIVATE ${CURSES_INCLUDE_DIR})
target_link_libraries(tomo PRIVATE ${CURSES_LIBRARIES})
target_link_libraries(tomo PRIVATE m)

install(TARGETS tomo RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_ts_grammar(c https://github.com/tree-sitter/tree-sitter-c.git)
add_ts_grammar(py https://github.com/tree-sitter/tree-sitter-python.git)
# For every new grammar add_ts_grammar(<name> <git-url>)
