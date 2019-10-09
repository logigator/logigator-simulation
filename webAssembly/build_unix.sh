#!/bin/bash

bash webAssembly/emscripten/emsdk activate latest
source ./webAssembly/emscripten/emsdk_env.sh || exit 1

em++ -O0 --bind \
 -std=c++11 \
 -s WASM=1 \
 -s ALLOW_MEMORY_GROWTH=1 \
 -o webAssembly/dist/logigator-simulation.js \
 -Isrc/ \
 -Isrc/components/ \
 src/wasm.cpp src/board.cpp src/input.cpp src/output.cpp src/link.cpp src/component.cpp || exit 1
