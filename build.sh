#!/usr/bin/env bash
#  just for me test
cd "$(dirname "$0")"

sudo rm -rf build

cmake -B build \
      -S . \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=/usr

cmake --build build

./build/src/cursor-ani
