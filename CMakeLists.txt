cmake_minimum_required(VERSION 3.10)
project(stark C)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_C_COMPILER /usr/bin/clang)

option(BUILD_TESTS off)
option(BUILD_MATH off)

add_library(stark_core STATIC src/core.c)
add_library(stark_opts STATIC src/opts.c)

target_include_directories(stark_core PUBLIC include)

target_link_libraries(stark_opts PUBLIC stark_core)

if(BUILD_TESTS)
  add_subdirectory(tests/opts)
endif()
