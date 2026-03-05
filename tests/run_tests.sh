#!/bin/bash

set -e

BUILD_DIR="./build/release/tests"

"$BUILD_DIR/shorthand"
printf "\n"
"$BUILD_DIR/longhand"

benchmarks=(
  benchmark_shorthand
  benchmark_longhand
  benchmark_longhand_equals
  benchmark_mixed
  )

for benchmark in "${benchmarks[@]}"; do
    printf "\n$benchmark:"
    perf stat -d "$BUILD_DIR/$benchmark"
done
