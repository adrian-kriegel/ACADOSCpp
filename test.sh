#!/usr/bin/env bash

cmake -S . -B build -DACADOS_CPP_BUILD_TESTS=ON
cmake --build build 


# Check if codegen dir exists
if [ ! -d "./codegen/test_model/build" ]; then
  echo "Generating test model"
  python ./test/generate_solver.py
fi


# Add test lib dir to LD_LIBRARY_PATH
script_dir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# ACADOS seems to install the library at somewhat arbitrary locations. 
test_model_install_dir=$(dirname $(find $script_dir -name "libacados_ocp_solver_test_model.so" | head -n 1))

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$test_model_install_dir

./build/test/testACADOSCpp
