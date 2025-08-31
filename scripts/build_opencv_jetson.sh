#!/bin/bash
# Script to build OpenCV 4.10.0 for Jetson with JetPack 6.1
# Based on https://github.com/AastaNV/JEP/blob/master/script/install_opencv4.10.0_Jetpack6.1.sh

set -e

OPENCV_VERSION=4.10.0
ARCH_BIN=8.7  # For Jetson with JetPack 6
INSTALL_DIR=/usr/local
DOWNLOAD_DIR=$(pwd)/opencv_build_temp
NPROC=$(nproc)

echo "Building OpenCV ${OPENCV_VERSION} for Jetson..."

# Check if OpenCV is already installed
if pkg-config --exists opencv4; then
    INSTALLED_VERSION=$(pkg-config --modversion opencv4)
    echo "OpenCV ${INSTALLED_VERSION} is already installed at system level"
    
    if [ "${INSTALLED_VERSION}" = "${OPENCV_VERSION}" ]; then
        echo "OpenCV ${OPENCV_VERSION} is already installed. Skipping build."
        exit 0
    else
        echo "Different version installed. Will build OpenCV ${OPENCV_VERSION}..."
    fi
fi

# Create temporary build directory
mkdir -p ${DOWNLOAD_DIR}
cd ${DOWNLOAD_DIR}

# Install dependencies
echo "Installing dependencies..."
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    unzip \
    pkg-config \
    libjpeg-dev \
    libpng-dev \
    libtiff-dev \
    libavcodec-dev \
    libavformat-dev \
    libswscale-dev \
    libgtk2.0-dev \
    libcanberra-gtk3-module \
    python3-dev \
    python3-numpy \
    python3-pip \
    libxvidcore-dev \
    libx264-dev \
    libtbb2 \
    libtbb-dev \
    libdc1394-dev \
    libv4l-dev \
    v4l-utils \
    libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    libavresample-dev \
    libvorbis-dev \
    libxine2-dev \
    libfaac-dev \
    libmp3lame-dev \
    libtheora-dev \
    libopencore-amrnb-dev \
    libopencore-amrwb-dev \
    libopenblas-dev \
    libatlas-base-dev \
    libblas-dev \
    liblapack-dev \
    libeigen3-dev \
    gfortran \
    libhdf5-dev \
    protobuf-compiler \
    libprotobuf-dev \
    libgoogle-glog-dev \
    libgflags-dev

# Download OpenCV and opencv_contrib
echo "Downloading OpenCV ${OPENCV_VERSION}..."
wget -O opencv.zip https://github.com/opencv/opencv/archive/${OPENCV_VERSION}.zip
wget -O opencv_contrib.zip https://github.com/opencv/opencv_contrib/archive/${OPENCV_VERSION}.zip

# Extract archives
echo "Extracting archives..."
unzip -q opencv.zip
unzip -q opencv_contrib.zip
rm opencv.zip opencv_contrib.zip

# Create build directory
cd opencv-${OPENCV_VERSION}
mkdir -p build
cd build

# Configure OpenCV build for Jetson
echo "Configuring OpenCV build..."
cmake -D CMAKE_BUILD_TYPE=RELEASE \
    -D CMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
    -D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib-${OPENCV_VERSION}/modules \
    -D EIGEN_INCLUDE_PATH=/usr/include/eigen3 \
    -D WITH_OPENCL=OFF \
    -D WITH_CUDA=ON \
    -D CUDA_ARCH_BIN=${ARCH_BIN} \
    -D CUDA_ARCH_PTX="" \
    -D WITH_CUDNN=ON \
    -D WITH_CUBLAS=ON \
    -D ENABLE_FAST_MATH=ON \
    -D CUDA_FAST_MATH=ON \
    -D OPENCV_DNN_CUDA=ON \
    -D ENABLE_NEON=ON \
    -D WITH_QT=OFF \
    -D WITH_OPENMP=ON \
    -D WITH_OPENGL=ON \
    -D BUILD_TIFF=ON \
    -D WITH_FFMPEG=ON \
    -D WITH_GSTREAMER=ON \
    -D WITH_TBB=ON \
    -D BUILD_TBB=ON \
    -D BUILD_TESTS=OFF \
    -D WITH_V4L=ON \
    -D WITH_LIBV4L=ON \
    -D OPENCV_ENABLE_NONFREE=ON \
    -D INSTALL_C_EXAMPLES=OFF \
    -D INSTALL_PYTHON_EXAMPLES=OFF \
    -D BUILD_NEW_PYTHON_SUPPORT=ON \
    -D BUILD_opencv_python3=ON \
    -D BUILD_opencv_python2=OFF \
    -D OPENCV_GENERATE_PKGCONFIG=ON \
    -D BUILD_EXAMPLES=OFF \
    -D BUILD_DOCS=OFF \
    -D BUILD_PERF_TESTS=OFF \
    -D BUILD_TESTS=OFF ..

# Build OpenCV
echo "Building OpenCV (this may take a while)..."
make -j${NPROC}

# Install OpenCV
echo "Installing OpenCV..."
sudo make install
sudo ldconfig

# Clean up
cd ../../..
rm -rf ${DOWNLOAD_DIR}

echo "OpenCV ${OPENCV_VERSION} has been successfully installed!"
echo "You can verify the installation with: pkg-config --modversion opencv4"