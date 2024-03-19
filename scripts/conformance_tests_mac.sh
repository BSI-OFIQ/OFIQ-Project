#!/bin/bash

pushd $PWD
cd ../build/build_mac/testing
./test_conformance_table --gtest_output=xml:"../reports/$t.xml"

popd
