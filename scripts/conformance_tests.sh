#!/bin/bash

pushd $PWD
cd ../build/build_linux/testing
./test_conformance_table --gtest_output=xml:"../reports/$t.xml"

popd
