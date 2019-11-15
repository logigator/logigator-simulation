@ECHO OFF

IF NOT EXIST webAssembly\emscripten\ (
    exit /b 1
)

@CALL webAssembly\emscripten\emsdk activate latest
@CALL webAssembly\emscripten\emsdk_env.bat --build=Release || exit /b 1

em++ -O3 --bind ^
 -std=c++11 ^
 -s WASM=1 ^
 -s ALLOW_MEMORY_GROWTH=1 ^
 -s "EXTRA_EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap', 'lengthBytesUTF8', 'stringToUTF8']" ^
 -o webAssembly/dist/logigator-simulation.js ^
 -Isrc/ ^
 -Isrc/components/ ^
 src/wasm.cpp src/board.cpp src/output.cpp || exit /b 1
