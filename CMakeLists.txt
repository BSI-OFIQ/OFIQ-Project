cmake_minimum_required(VERSION 3.26)

file(READ "Version.txt" ver)

string(REGEX MATCH "VERSION_MAJOR ([0-9]*)" _ ${ver})
set(ver_major ${CMAKE_MATCH_1})

string(REGEX MATCH "VERSION_MINOR ([0-9]*)" _ ${ver})
set(ver_minor ${CMAKE_MATCH_1})

string(REGEX MATCH "VERSION_PATCH ([0-9]*)" _ ${ver})
set(ver_patch ${CMAKE_MATCH_1})

string(REGEX MATCH "VERSION_SUFFIX ([0-9a-zA-Z]*)" _ ${ver})
set(ver_suffix ${CMAKE_MATCH_1})
set(PROJECT_VERSION_SUFFIX ${ver_suffix})

message("version: ${ver_major}.${ver_minor}.${ver_patch}")

option(DOWNLOAD_ONNX "Whether ONNX must be downloaded" ON)
option(DOWNLOAD_MODELS_AND_IMAGES "Whether model and image files must be downloaded" OFF)

SET(USE_CONAN ON CACHE BOOL "If conan should be used or not")
SET(BUILD_TYPE Release CACHE STRING "Build Debug or Release")
SET(ARCHITECTURE x64 CACHE STRING "x64 or Win32 for Windows")
SET(OS windows CACHE STRING "Operating System windows, linux or mac")

include(cmake/DownloadDependencies.cmake)

if(UNIX AND NOT APPLE)
    project(OFIQLib VERSION ${ver_major}.${ver_minor}.${ver_patch} LANGUAGES CXX)

    if(DOWNLOAD_ONNX)
        set(ONNX_FILE onnxruntime-linux-x64-1.17.3.tgz)
        set(ONNX_URL https://github.com/microsoft/onnxruntime/releases/download/v1.17.3/onnxruntime-linux-x64-1.17.3.tgz)
        message("Downloading ONNX from ${ONNX_URL}")
        download_file(${ONNX_URL}  ${ONNX_FILE})
    endif(DOWNLOAD_ONNX)

    include(OFIQlib/CMakeLists.ubuntu.conan.cmake)
elseif(APPLE)
    project(OFIQLibMac VERSION ${ver_major}.${ver_minor}.${ver_patch} LANGUAGES CXX)

    if(DOWNLOAD_ONNX)
        set(ONNX_FILE onnxruntime-osx-universal2-1.17.3.tgz)
        set(ONNX_URL https://github.com/microsoft/onnxruntime/releases/download/v1.17.3/onnxruntime-osx-universal2-1.17.3.tgz)
        message("Downloading ONNX from ${ONNX_URL}")
        download_file(${ONNX_URL}  ${ONNX_FILE})
    endif(DOWNLOAD_ONNX)
    include(OFIQlib/CMakeLists.macos.cmake)
else(APPLE)
    project(OFIQLibWin VERSION ${ver_major}.${ver_minor}.${ver_patch} LANGUAGES CXX)

    if(DOWNLOAD_ONNX)
        if( ARCHITECTURE STREQUAL "x64" )
            set(ONNX_FILE onnxruntime-win-x64-1.17.3.zip)
            set(ONNX_URL https://github.com/microsoft/onnxruntime/releases/download/v1.17.3/onnxruntime-win-x64-1.17.3.zip)
            message("Downloading ONNX from ${ONNX_URL}")
            download_file(${ONNX_URL}  ${ONNX_FILE})
        else ()
            set(ONNX_FILE onnxruntime-win-x86-1.17.3.zip)
            set(ONNX_URL https://github.com/microsoft/onnxruntime/releases/download/v1.17.3/onnxruntime-win-x86-1.17.3.zip)
            message("Downloading ONNX from ${ONNX_URL}")
            download_file(${ONNX_URL}  ${ONNX_FILE})
        endif()
    endif(DOWNLOAD_ONNX)

    include(OFIQlib/CMakeLists.windows.cmake)
endif (UNIX AND NOT APPLE)

if(DOWNLOAD_MODELS_AND_IMAGES)
    set(MODEL_FILE OFIQ-MODELS.zip)
    set(MODEL_URL https://standards.iso.org/iso-iec/29794/-5/ed-1/en/OFIQ-MODELS.zip)
    message("Downloading models from ${MODEL_URL}")
    file(DOWNLOAD ${MODEL_URL}  ${MODEL_FILE})
    message("Extracting models to ${PROJECT_SOURCE_DIR}/data")
    file(ARCHIVE_EXTRACT 
        INPUT ${MODEL_FILE}
        DESTINATION ${PROJECT_SOURCE_DIR}/data
        VERBOSE
    )
    file(REMOVE ${MODEL_FILE})

    set(IMAGES_FILE OFIQ-IMAGES.zip)
    set(IMAGES_URL https://standards.iso.org/iso-iec/29794/-5/ed-1/en/OFIQ-IMAGES.zip)
    message("Downloading images from ${IMAGES_URL}")
    file(DOWNLOAD ${IMAGES_URL}  ${IMAGES_FILE})
    message("Extracting images to ${PROJECT_SOURCE_DIR}/data/tests")
    file(ARCHIVE_EXTRACT 
        INPUT ${IMAGES_FILE}
        DESTINATION ${PROJECT_SOURCE_DIR}/data/tests
        VERBOSE
    )
    file(REMOVE ${IMAGES_FILE})
endif(DOWNLOAD_MODELS_AND_IMAGES)

# starting : Tests
include(CTest)
enable_testing()
add_subdirectory(testing)
