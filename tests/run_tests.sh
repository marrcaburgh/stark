#!/bin/bash

set -e

BUILD_DIR="./build/release/tests"

"$BUILD_DIR/shorthand"
printf "\n"
"$BUILD_DIR/longhand"
"$BUILD_DIR/positional"

for f in ./tests/*.c; do
    benchmark=$(basename "$f" .c)
    printf "\n$benchmark:\n"
    perf stat -d "$BUILD_DIR/$benchmark"
done
