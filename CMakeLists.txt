cmake_minimum_required(VERSION 3.10)
project(stark C)

if(COMPILE_CLI_OPTS)
  add_subdirectory(source/tests/cli_opts)
endif()

if(COMPILE_HASH_TABLE)
  add_subdirectory(source/tests/hash_table)
endif()
