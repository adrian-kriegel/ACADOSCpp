#!/usr/bin/env bash

cmake -S . -B build -DACADOS_CPP_BUILD_TESTS=ON
cmake --build build 

# Add test lib dir to LD_LIBRARY_PATH
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SCRIPT_DIR/test/lib

./build/test/testACADOSCpp
