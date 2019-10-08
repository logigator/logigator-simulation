@ECHO OFF

IF EXIST logigator-simulation.wasm (
	exit /b 0
)

git clone https://github.com/emscripten-core/emsdk.git WASM/emscripten || exit /b 1

@CALL emscripten\emsdk install latest


