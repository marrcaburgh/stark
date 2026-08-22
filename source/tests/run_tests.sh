#!/bin/bash

set -e

exec > >(stdbuf -o0 cat)

if [ "$#" -ne 1 ]; then
  echo "incorrect number of arguments"

  exit 16
else
  case "$1" in
  "cli_opts")
    cmake --preset release-cli_opts
    cmake --build --preset release-cli_opts
    ;;
  "hash_table")
    cmake --preset release-hash_table
    cmake --build --preset release-hash_table
    ;;
  *)
    echo "invalid library name (expected 'cli_opts' or 'hash_table')"

    exit 13
    ;;
  esac

  BUILD_DIR="./build/release-$1/source/tests/$1"
fi

shopt -s nullglob

for f in "${BUILD_DIR}"/test_*; do
  printf "\n%s:\n" "${f}"
  "${f}"
done

for f in "${BUILD_DIR}"/benchmark_*; do
  perf stat -d --repeat 10 -- "${f}"
done
