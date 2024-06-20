#!/bin/bash
build_dir=build/build_linux

if [ "$1" = "--os" ]; then
    shift
    if [ "$1" = "macos" ]; then
        build_dir=build/build_mac
    else
        echo "$1" is a not a supported OS
        exit
    fi
fi

cd ../${build_dir}/testing
./test_conformance_table --gtest_output=xml:"../reports/$t.xml"

