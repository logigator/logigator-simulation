#!/bin/bash

if [ -f 'webAssembly/dist/logigator-simulation.wasm' ]; then
  exit 0
fi

if [ -d 'webAssembly/emscripten' ]; then
  git clone https://github.com/emscripten-core/emsdk.git webAssembly/emscripten || exit 1
  ./webAssembly/emscripten/emsdk install latest
fi

./webAssembly/build_unix.sh
