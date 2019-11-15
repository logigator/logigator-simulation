#!/bin/bash

if [ ! -d 'webAssembly/emscripten' ]; then
  exit 1
fi

bash webAssembly/emscripten/emsdk activate latest
source ./webAssembly/emscripten/emsdk_env.sh || exit 1

em++ -O3 --bind \
 -std=c++11 \
 -s WASM=1 \
 -s ALLOW_MEMORY_GROWTH=1 \
 -s 'EXTRA_EXPORTED_RUNTIME_METHODS=["ccall", "cwrap", "lengthBytesUTF8", "stringToUTF8"]' \
 -o webAssembly/dist/logigator-simulation.js \
 -Isrc/ \
 -Isrc/components/ \
 src/wasm.cpp src/board.cpp src/output.cpp || exit 1
