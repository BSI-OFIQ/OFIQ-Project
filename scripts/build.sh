#!/bin/bash
build_dir=build/build_linux
install_dir=install_x86_64_linux
use_conan=ON
use_system_opencv=OFF
download=ON
config=Release
os=linux
useSonarWrapper=false
sonarWrapper="${SONARQUBE_WRAPPER_PATH:-.sonar}/build-wrapper-linux-x86-64"

# Check if running on Jetson
is_jetson=false
if [ -f /etc/nv_tegra_release ]; then
    is_jetson=true
    echo "Detected Jetson platform"
    # On Jetson, default to using system OpenCV
    use_system_opencv=ON
    use_conan=OFF
fi

while [ -n "$1" ]
do  
if [ "$1" = "--no-conan" ]; then
    use_conan=OFF
elif [ "$1" = "--no-download" ]; then
    download=OFF
elif [ "$1" = "--use-system-opencv" ]; then
    use_system_opencv=ON
    use_conan=OFF
elif [ "$1" = "--debug" ]; then
    config=Debug
elif [ "$1" = "--use-sonar-wrapper" ]; then
    useSonarWrapper=true
    echo "The following sonar wrapper will be used: $sonarWrapper"
elif [ "$1" = "--os" ]; then
    shift
    if [ "$1" = "macos" ]; then
        build_dir=build/build_mac
        install_dir=install_arm64_mac
        os=macos
    elif [ "$1" = "linux-arm64" ]; then
        build_dir=build/build_linux_arm64
        install_dir=install_arm64_linux
        os=linux_arm64
        # On ARM64, check if it's Jetson and use system OpenCV
        if [ "$is_jetson" = "true" ]; then
            use_system_opencv=ON
            use_conan=OFF
        fi
    else
        echo "$1" is a not a supported OS
        exit
    fi
else
    echo "$1" is not a supported argument
    exit
fi
shift
done

if [ "$use_conan" = "ON" ]
then
    rm -rf ../build/conan

    export OFIQLIB_CONAN_DIR=../conan
    export CONAN_FILE=conanfile.txt

    echo using conan file ${CONAN_FILE}

    if [ "$config" = "Release" ]
    then
        conan install ${OFIQLIB_CONAN_DIR}/${CONAN_FILE} \
            --build missing \
            --profile:build ${OFIQLIB_CONAN_DIR}/conan_profile_release_${os}.txt \
            --profile:host ${OFIQLIB_CONAN_DIR}/conan_profile_release_${os}.txt \
            --output-folder=../build/conan \
            -g CMakeDeps \
            -g CMakeToolchain
    else
        conan install ${OFIQLIB_CONAN_DIR}/${CONAN_FILE} \
        --build missing \
        --profile:build ${OFIQLIB_CONAN_DIR}/conan_profile_debug_${os}.txt \
        --profile:host ${OFIQLIB_CONAN_DIR}/conan_profile_debug_${os}.txt \
        --output-folder=../build/conan \
        -g CMakeDeps \
        -g CMakeToolchain
    fi
else
    # If using system OpenCV, skip building OpenCV
    if [ "$use_system_opencv" = "ON" ]; then
        echo "Using system-installed OpenCV"
        
        # On Jetson, check if OpenCV needs to be built
        if [ "$is_jetson" = "true" ]; then
            if ! pkg-config --exists opencv4; then
                echo "OpenCV not found on Jetson. Building OpenCV 4.10.0..."
                cd ../scripts
                chmod +x build_opencv_jetson.sh
                ./build_opencv_jetson.sh || exit 1
                cd ..
            else
                OPENCV_VERSION=$(pkg-config --modversion opencv4)
                echo "Using existing OpenCV ${OPENCV_VERSION}"
            fi
        else
            # For non-Jetson systems, just check if OpenCV exists
            if ! pkg-config --exists opencv4; then
                echo "ERROR: System OpenCV not found. Please install OpenCV 4.x"
                exit 1
            fi
            OPENCV_VERSION=$(pkg-config --modversion opencv4)
            echo "Using system OpenCV ${OPENCV_VERSION}"
        fi
        
        # Still need to download other external dependencies (non-OpenCV)
        echo "Downloading external dependencies (non-OpenCV)..."
        chmod +x ../scripts/download_dependencies.sh
        ../scripts/download_dependencies.sh
    else
        # Original behavior: download and build everything
        if [ "$download" = "ON" ]
        then
            cmake -P ../cmake/DownloadExternalSourceCode.cmake
        fi
        # build opencv
        cd ../extern/opencv-4.5.5
    cmake -S ./ -B build -DBUILD_LIST=core,calib3d,imgcodecs,improc,dnn,ml \
    -DBUILD_opencv_apps=OFF -DBUILD_opencv_java=OFF -DBUILD_opencv_python=OFF -DWITH_FFMPEG=OFF -DWITH_TIFF=OFF -DWITH_WEBP=OFF -DWITH_IPP=OFF \
    -DWITH_OPENCL=OFF -DWITH_LAPACK=OFF -DWITH_QUIRC=OFF \
    -DBUILD_ZLIB=ON -DWITH_ZLIB=ON \
    -DBUILD_PNG=ON -DWITH_PNG=ON \
    -DBUILD_JPEG=ON -DWITH_JPEG=ON \
    -DBUILD_OPENEXR=OFF -DWITH_OPENEXR=OFF \
    -DBUILD_SHARED_LIBS=OFF \
    -DWITH_ADE=OFF -DWITH_CAROTENE=OFF \
    -DCMAKE_INSTALL_PREFIX=./build/install || exit 1
    cmake --build build --config $config --target install -j 8 || exit 1
        # build gtest
        cd ../googletest
    fi
    
    # Build gtest regardless of OpenCV choice (needed for tests)
    if [ -d "../extern/googletest" ]; then
        echo "Building Google Test..."
        cd ../extern/googletest
        cmake -S ./ -B build -DBUILD_GMOCK=OFF -DINSTALL_GTEST=OFF -DBUILD_SHARED_LIBS=ON || exit 1
        cmake --build build --config $config || exit 1
        cd ../../scripts  # Go back to scripts directory
    fi
    
    # Build onnxruntime only if not using system OpenCV
    if [ "$use_system_opencv" = "OFF" ]; then
        if [ -d "../extern/onnxruntime" ]; then
            cd ../extern/onnxruntime
            sh ./build.sh --config $config --build_shared_lib --parallel --compile_no_warning_as_error --skip_submodule_sync --update --build || exit 1
            cd ..
        fi
    fi
fi

echo "Attempting to build the real implementation"

cd ../
echo "Removing $build_dir"
rm -r $build_dir

echo "Generating build files"
cmake -S ./ -B $build_dir -DCMAKE_INSTALL_PREFIX=$install_dir -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON \
    -DDOWNLOAD_ONNX=$use_conan -DUSE_CONAN=$use_conan -DUSE_SYSTEM_OPENCV=$use_system_opencv \
    -DOS=$os -DCMAKE_BUILD_TYPE=$config -DDOWNLOAD_MODELS_AND_IMAGES=$download

cmakeBuildCommand="cmake --build $build_dir --target install -j 8"
if [ "$useSonarWrapper" = "true" ]
then
    $sonarWrapper --out-dir bw-output $cmakeBuildCommand
else
    $cmakeBuildCommand
fi

echo "Building finished"
