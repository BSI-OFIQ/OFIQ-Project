#!/bin/bash
build_dir=build/build_linux

if [ "$1" = "--os" ]; then
    shift
    if [ "$1" = "macos" ]; then
        build_dir=build/build_mac
    elif [ "$1" = "linux-arm64" ]; then
        build_dir=build/build_linux_arm64
    else
        echo "$1 is a not a supported OS"
        exit 1
    fi
    shift
fi

cd ../${build_dir}/testing

mkdir -p ../reports

for test_binary in \
    test_config_defaults_jaxn \
    test_conformance_table_jaxnconfig \
    test_conformance_table_inmemoryconfig
do
    ./"${test_binary}" --gtest_output=xml:"../reports/${test_binary}.xml" || exit $?
done
