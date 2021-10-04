add_compile_definitions(XEUS_EMSCRIPTEN_WASM_BUILD)

set(XEUS_EMSCRIPTEN_WASM_FLAGS "${XEUS_EMSCRIPTEN_WASM_FLAGS} --bind")
set(XEUS_EMSCRIPTEN_WASM_FLAGS "${XEUS_EMSCRIPTEN_WASM_FLAGS} -Wno-deprecated -Wno-unused-command-line-argument")
#set(XEUS_EMSCRIPTEN_WASM_FLAGS "${XEUS_EMSCRIPTEN_WASM_FLAGS} -lwebsocket.js")
# module / import 
set(XEUS_EMSCRIPTEN_WASM_FLAGS "${XEUS_EMSCRIPTEN_WASM_FLAGS} -s MODULARIZE=1 -s EXPORT_NAME=\"createXeusModule\" -s EXPORT_ES6=0 -s USE_ES6_IMPORT_META=0")
# safty
set(XEUS_EMSCRIPTEN_WASM_FLAGS "${XEUS_EMSCRIPTEN_WASM_FLAGS}  -s  DEMANGLE_SUPPORT=0 -s SAFE_HEAP=0  -s ASSERTIONS=0   -s STACK_OVERFLOW_CHECK=0 ")
# web
#set(XEUS_EMSCRIPTEN_WASM_FLAGS "${XEUS_EMSCRIPTEN_WASM_FLAGS} -s ENVIRONMENT=web,worker")
# mem
set(XEUS_EMSCRIPTEN_WASM_FLAGS "${XEUS_EMSCRIPTEN_WASM_FLAGS} -s ALLOW_MEMORY_GROWTH=1")
set(XEUS_EMSCRIPTEN_WASM_FLAGS "${XEUS_EMSCRIPTEN_WASM_FLAGS} -s EXIT_RUNTIME=1")
set(XEUS_EMSCRIPTEN_WASM_FLAGS "${XEUS_EMSCRIPTEN_WASM_FLAGS} -s WASM=1")
set(XEUS_EMSCRIPTEN_WASM_FLAGS "${XEUS_EMSCRIPTEN_WASM_FLAGS} -s ERROR_ON_UNDEFINED_SYMBOLS=0")
# threads
set(XEUS_EMSCRIPTEN_WASM_FLAGS "${XEUS_EMSCRIPTEN_WASM_FLAGS} -s USE_PTHREADS=1")
# exceptions    
set(XEUS_EMSCRIPTEN_WASM_FLAGS "${XEUS_EMSCRIPTEN_WASM_FLAGS} -s DISABLE_EXCEPTION_CATCHING=0 ")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${XEUS_EMSCRIPTEN_WASM_FLAGS}")
set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS}  ${XEUS_EMSCRIPTEN_WASM_FLAGS}  -s USE_PTHREADS=1 ")