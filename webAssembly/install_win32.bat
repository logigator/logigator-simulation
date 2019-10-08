@ECHO OFF

IF EXIST webAssembly\dist\logigator-simulation.wasm (
	exit /b 0
)

IF NOT EXIST webAssembly\emscripten\ (
    git clone https://github.com/emscripten-core/emsdk.git webAssembly/emscripten || exit /b 1
    @CALL webAssembly\emscripten\emsdk install latest
)

@CALL webAssembly\build_win32.bat
