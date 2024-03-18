#!/bin/bash

rm -rf ../build/conan

export OFIQLIB_CONAN_DIR=../conan
export CONAN_FILE=conanfile.txt

echo using conan file ${CONAN_FILE}

conan install ${OFIQLIB_CONAN_DIR}/${CONAN_FILE} \
        --build missing \
        --profile:build ${OFIQLIB_CONAN_DIR}/conan_profile_release_mac.txt \
        --profile:host ${OFIQLIB_CONAN_DIR}/conan_profile_release_mac.txt \
        --output-folder=../build/conan \
        -g CMakeDeps \
        -g CMakeToolchain


build_dir=build/build_mac
install_dir=install_x86_64_mac

echo "Attempting to build the real implementation"

cd ../
echo "Removing $build_dir"
rm -r $build_dir

echo "Generating build files"
cmake -S ./ -B $build_dir -DCMAKE_INSTALL_PREFIX=$install_dir -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DDOWNLOAD_MODELS=ON -DDOWNLOAD_IMAGES=ON
cmake --build $build_dir --target install -j 8

echo "Building finished"
