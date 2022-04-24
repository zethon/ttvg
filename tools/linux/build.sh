#!/bin/bash
#
#
#

GIT_ROOT="$(git rev-parse --show-toplevel)"

if [ "$1" == "clean" ]; then
    rm -rf "$GIT_ROOT/build"
    exit 0
fi

if [ ! -d "$GIT_ROOT/build" ]; then
    mkdir "$GIT_ROOT/build"
fi

#
# Add submodules
#
git submodule update --init --recursive

cd "$GIT_ROOT/build"

conan install .. -s build_type=Debug --build=missing
cmake .. -DCMAKE_BUILD_TYPE=Debug -D CMAKE_C_COMPILER=/usr/bin/gcc-10 -D CMAKE_CXX_COMPILER=/usr/bin/g++-10 .
cmake --build .

