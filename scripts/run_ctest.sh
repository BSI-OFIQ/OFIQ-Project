#!/bin/bash

pushd $PWD
cd ../build/build_linux
ctest
# ctest --rerun-failed --output-on-failure
popd
