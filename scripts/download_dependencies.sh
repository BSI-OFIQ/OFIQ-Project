#!/bin/bash
# Script to download missing external dependencies for OFIQ when using system OpenCV

set -e

cd "$(dirname "$0")/.."
EXTERN_DIR="$(pwd)/extern"

echo "Downloading external dependencies to $EXTERN_DIR..."

# Create directories if they don't exist
mkdir -p $EXTERN_DIR/{json,magic_enum,PEGTL,flatbuffers,di,abseil-cpp,googletest,thirdParty}

# Download TAO JSON
if [ ! -f "$EXTERN_DIR/json/include/tao/json/forward.hpp" ]; then
    echo "Downloading TAO JSON..."
    cd $EXTERN_DIR
    git clone --depth 1 --branch 1.0.0-beta.13 https://github.com/taocpp/json.git taocpp-json-temp
    mkdir -p json/include
    cp -r taocpp-json-temp/include/* json/include/
    rm -rf taocpp-json-temp
    cd ..
fi

# Download magic_enum
if [ ! -f "$EXTERN_DIR/magic_enum/include/magic_enum/magic_enum.hpp" ]; then
    echo "Downloading magic_enum..."
    cd $EXTERN_DIR
    git clone --depth 1 --branch v0.8.1 https://github.com/Neargye/magic_enum.git magic_enum-temp
    mkdir -p magic_enum/include/magic_enum
    cp -r magic_enum-temp/include/* magic_enum/include/
    rm -rf magic_enum-temp
    cd ..
fi

# Download PEGTL
if [ ! -f "$EXTERN_DIR/PEGTL/include/tao/pegtl.hpp" ]; then
    echo "Downloading PEGTL..."
    cd $EXTERN_DIR
    git clone --depth 1 --branch 3.2.7 https://github.com/taocpp/PEGTL.git PEGTL-temp
    mkdir -p PEGTL/include
    cp -r PEGTL-temp/include/* PEGTL/include/
    rm -rf PEGTL-temp
    cd ..
fi

# Download Flatbuffers
if [ ! -d "$EXTERN_DIR/flatbuffers/include" ]; then
    echo "Downloading Flatbuffers..."
    cd $EXTERN_DIR
    git clone --depth 1 --branch v2.0.8 https://github.com/google/flatbuffers.git flatbuffers-temp
    mkdir -p flatbuffers/include
    cp -r flatbuffers-temp/include/* flatbuffers/include/
    rm -rf flatbuffers-temp
    cd ..
fi

# Download Google Test
if [ ! -f "$EXTERN_DIR/googletest/CMakeLists.txt" ]; then
    echo "Downloading Google Test..."
    cd $EXTERN_DIR
    rm -rf googletest
    git clone --depth 1 --branch release-1.12.1 https://github.com/google/googletest.git
    cd ..
fi

echo "Dependencies downloaded successfully!"