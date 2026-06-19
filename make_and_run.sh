#!/bin/bash

set -e

if [ ! -d "build" ]; then
  cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
fi

cd build/ && make && cd .. && ./stylize-animation
