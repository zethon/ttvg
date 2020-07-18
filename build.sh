#!/bin/bash
#
#
#

if [ "$1" == "clean" ]; then
    rm -rf build
    exit 0
fi

if [ ! -f "build" ]; then
    mkdir build
fi

cd build

conan install .. -s build_type=Debug --build=missing
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .

