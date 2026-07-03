#!/bin/bash

set -e

BUILD_DIR="./build/release/source/tests/opts"

for f in ${BUILD_DIR}/test_*; do
  printf "\n${f}:\n"
  ./${f}
done

for f in ${BUILD_DIR}/benchmark_*; do
    perf stat -d --repeat 3 "${f}"
done
