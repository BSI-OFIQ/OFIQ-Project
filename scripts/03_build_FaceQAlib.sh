#!/bin/bash

build_dir=build/build_linux
#platform_name=x64  

cd ../
cmake --build $build_dir --target install
echo "Building finished"
