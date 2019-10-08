#!/bin/bash

if [ -f 'logigator-simulation.wasm' ]; then
  exit 0
fi

git clone https://github.com/emscripten-core/emsdk.git emscripten || exit 1

./emscripten/emsdk install latest

bash build_unix.sh
