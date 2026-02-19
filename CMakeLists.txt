cmake_minimum_required(VERSION 3.10)
project(cli_opts C)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_C_COMPILER /usr/bin/clang)

option(BUILD_TESTS off)

add_library(cli_opts STATIC src/cli_opts.c)

target_include_directories(cli_opts PUBLIC include)

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
