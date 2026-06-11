#!/bin/bash

set -e

BUILD_DIR="./build/release/tests/opts"

"$BUILD_DIR/shorthand"
"$BUILD_DIR/longhand"
"$BUILD_DIR/positional"

for f in ./tests/opts/benchmark_*.c; do
    benchmark=$(basename "$f" .c)
    printf "\n$benchmark:\n"
    perf stat -d "$BUILD_DIR/$benchmark"
done
