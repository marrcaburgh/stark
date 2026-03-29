#!/bin/bash

set -e

BUILD_DIR="./build/release/tests/opts"

"$BUILD_DIR/shorthand"
printf "\n"
"$BUILD_DIR/longhand"
"$BUILD_DIR/positional"

for f in ./tests/benchmark_*.c; do
    benchmark=$(basename "$f" .c)
    printf "\n$benchmark:\n"
    perf stat -e cycles,instructions,cache-misses,branch-misses "$BUILD_DIR/$benchmark"
done
