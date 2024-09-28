#!/usr/bin/env bash

cmake -S . -B build -DACADOS_CPP_BUILD_TESTS=ON
cmake --build build 

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SCRIPT_DIR/test/lib

echo $LD_LIBRARY_PATH

./build/test/testACADOSCpp
