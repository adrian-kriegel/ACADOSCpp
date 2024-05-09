#!/bin/env bash

cd build
cmake .. 
make testACADOSCpp -j4
cd ..

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SCRIPT_DIR/test/lib

echo $LD_LIBRARY_PATH

./build/test/testACADOSCpp
