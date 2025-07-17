cmake_minimum_required(VERSION 3.5)
project(tomo C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/include)

AUX_SOURCE_DIRECTORY(src SRC)

add_executable(tomo ${SRC})
target_link_libraries(tomo ${CURSES_LIBRARIES} m)

