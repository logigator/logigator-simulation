@ECHO OFF

@CALL webAssembly\emscripten\emsdk activate latest
@CALL webAssembly\emscripten\emsdk_env.bat --build=Release || exit /b 1

em++ -O0 --bind -s WASM=1 ^
 -s MODULARIZE=1 ^
 -s EXPORT_NAME=SimulationModule ^
 -s ALLOW_MEMORY_GROWTH=1 ^
 -s MALLOC=emmalloc ^
 -o webAssembly/dist/logigator-simulation.js ^
 webAssembly/test.cpp || exit /b 1
