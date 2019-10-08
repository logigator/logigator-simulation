#!/bin/bash

if [ -f 'webAssembly/dist/logigator-simulation.wasm' ]; then
  exit 0
fi

if [ -d 'webAssembly/emscripten' ]; then
  git clone https://github.com/emscripten-core/emsdk.git webAssembly/emscripten || exit 1
  bash webAssembly/emscripten/emsdk install latest
fi

bash webAssembly/build_unix.sh
