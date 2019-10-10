@ECHO OFF

IF NOT EXIST webAssembly\emscripten\ (
    exit /b 1
)

@CALL webAssembly\emscripten\emsdk activate latest
@CALL webAssembly\emscripten\emsdk_env.bat --build=Release || exit /b 1

em++ -O0 --bind ^
 -std=c++11 ^
 -s WASM=1 ^
 -s ALLOW_MEMORY_GROWTH=1 ^
 -o webAssembly/dist/logigator-simulation.js ^
 -Isrc/ ^
 -Isrc/components/ ^
 src/wasm.cpp src/board.cpp src/input.cpp src/output.cpp src/link.cpp src/component.cpp || exit /b 1
