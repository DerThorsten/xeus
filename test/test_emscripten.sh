#!/bin/bash
set -e
pwd


if [ $1 = "BUILD" ]; then
    echo "BUILD: docker build:"
    docker build -f TestWasmDockerfile -t test_xeus_wasm  --build-arg USER_ID=$(id -u) --build-arg GROUP_ID=$(id -g) .
    docker run --rm -v $(pwd):/xeus    -u $(id -u):$(id -g)  test_xeus_wasm    /xeus/test/copy_files.sh
else
    echo "SKIP BUILD:"
fi

echo "Import with Node"
node test/emscripten_wasm_test.js


echo "Test with Node"
cd test
node test_wasm.js