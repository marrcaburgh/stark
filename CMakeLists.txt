cmake_minimum_required(VERSION 3.10)
project(mb_clite C)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_C_COMPILER /usr/bin/clang)

option(BUILD_TESTS off)

add_library(mb_clite_opts STATIC src/mb_clite_opts.c)

target_include_directories(mb_clite_opts PUBLIC include)

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
