#!/bin/bash

set -e

export OPTIMIZE="-Os"
export LDFLAGS="${OPTIMIZE}"
export CFLAGS="${OPTIMIZE}"
export CPPFLAGS="${OPTIMIZE}"

echo "thepwd"  $PWD
mkdir -p dist
cd /embuild/test_wasm
pwd
echo "root"
ls
ls
cp fobar.{js,wasm} /src/dist
# echo "============================================="
# echo "Compiling wasm bindings"
# echo "============================================="
# (
#   # Compile C/C++ code
#   emcc \
#     ${OPTIMIZE} \
#     --bind \
#     -s STRICT=1 \
#     -s ALLOW_MEMORY_GROWTH=1 \
#     -s ASSERTIONS=0 \
#     -s MALLOC=emmalloc \
#     -s MODULARIZE=1 \
#     -s EXPORT_ES6=1 \
#     -o ./my-module.js \
#     -I /opt/libvpx/src \
#     my-module.cpp \
#     /opt/libvpx/build/libvpx.a

#   # Create output folder
#   mkdir -p dist
#   # Move artifacts
#   mv my-module.{js,wasm} dist
# )
echo "============================================="
echo "Compiling wasm bindings done"
echo "============================================="