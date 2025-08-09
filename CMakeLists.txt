cmake_minimum_required(VERSION 3.16)
project(tomo C)
set(CMAKE_C_STANDARD 11)

include(FetchContent)
include(GNUInstallDirs)

FetchContent_Declare(
  tree_sitter
  GIT_REPOSITORY https://github.com/tree-sitter/tree-sitter.git
  GIT_TAG main
)
FetchContent_GetProperties(tree_sitter)
if(NOT tree_sitter_POPULATED)
  FetchContent_Populate(tree_sitter)
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
  add_library(ts_${GRAMMAR_NAME} SHARED ${parser_files})
  target_include_directories(ts_${GRAMMAR_NAME} PRIVATE ${src_dir}/src ${tree_sitter_SOURCE_DIR}/lib/include)
  target_link_libraries(ts_${GRAMMAR_NAME} PRIVATE tree_sitter)
  set_target_properties(ts_${GRAMMAR_NAME} PROPERTIES OUTPUT_NAME "tree-sitter-${GRAMMAR_NAME}" PREFIX "" LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/parsers)
  install(TARGETS ts_${GRAMMAR_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/tomo-parsers)
endfunction()

add_executable(tomo src/main.c)
target_link_libraries(tomo PRIVATE tree_sitter)
install(TARGETS tomo RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_ts_grammar(c https://github.com/tree-sitter/tree-sitter-c.git)
# add_ts_grammar(python https://github.com/tree-sitter/tree-sitter-python.git)
# For every new grammar add_ts_grammar(<name> <git-url>)
