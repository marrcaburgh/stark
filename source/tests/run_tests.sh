#!/bin/bash

set -e

BUILD_DIR="./build/release/source/tests/"

if [ "$#" -ne 1 ]; then
  echo "incorrect number of arguments"

  exit 16
else

  case "$1" in
    "cli_opts")
      BUILD_DIR+="cli_opts"
      cmake --preset release -DCOMPILE_CLI_OPTS=ON -DCOMPILE_HASH_TABLE=ON

      ;;
    "hash_table")
      BUILD_DIR+="hash_table"

      cmake --preset release -DCOMPILE_HASH_TABLE=ON
      ;;
    *)
      echo "invalid library name (expected 'cli_opts' or 'hash_table')"

      exit 13
      ;;
  esac
fi

cmake --build --preset release --clean-first

shopt -s nullglob

for f in "${BUILD_DIR}"/test_*; do
  printf "\n%s:\n" "${f}"
  "${f}"
done

for f in "${BUILD_DIR}"/benchmark_*; do
    perf stat -d --repeat 3 -- "${f}"
done
