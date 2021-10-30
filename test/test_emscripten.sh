#!/bin/bash
pwd
docker build -f TestWasmDockerfile -t test_xeus_wasm  --build-arg USER_ID=$(id -u) --build-arg GROUP_ID=$(id -g) . 
docker run --rm -v $(pwd):/xeus    -u $(id -u):$(id -g)  test_xeus_wasm    /xeus/test/copy_files.sh

echo "IMPORT WITH NODE"
node test/emscripten_wasm_test.js


echo "Import in noide"
cd test
node test_wasm.js