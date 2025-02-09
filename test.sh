#!/usr/bin/env bash

cmake -S . -B build -DACADOS_CPP_BUILD_TESTS=ON
cmake --build build 


# Check if codegen dir exists
if [ ! -d "./codegen/test_model/build" ]; then
  echo "Generating test model"
  python ./test/generate_solver.py
fi

# Add test lib dir to LD_LIBRARY_PATH
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SCRIPT_DIR/codegen/test_model/build

./build/test/testACADOSCpp
