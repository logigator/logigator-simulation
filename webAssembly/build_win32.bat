@ECHO OFF

@CALL emscripten\emsdk activate latest
@CALL emscripten\emsdk_env.bat --build=Release || exit /b 1

em++ -O0 --bind -s WASM=1 ^
 -s MODULARIZE=1 ^
 -s EXPORT_NAME=SimulationModule ^
 -s ALLOW_MEMORY_GROWTH=1 ^
 -s MALLOC=emmalloc ^
 -o logigator-simulation.js ^
 ../src/ || exit /b 1
