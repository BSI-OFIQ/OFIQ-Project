#project(FaceQALib)
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

include(cmake/DownloadDependencies.cmake)

if (UNIX)
    project(FaceQALib VERSION ${ver_major}.${ver_minor}.${ver_patch} LANGUAGES CXX)

    set(ONNX_FILE onnxruntime-linux-x64-1.14.1.tgz)
    set (ONNX_URL https://github.com/microsoft/onnxruntime/releases/download/v1.14.1/onnxruntime-linux-x64-1.14.1.tgz)
    download_file(${ONNX_URL}  ${ONNX_FILE})

    include(FaceQAlib/CMakeLists.ubuntu.conan.cmake)
else(UNIX)
    project(FaceQALibWin VERSION ${ver_major}.${ver_minor}.${ver_patch} LANGUAGES CXX)

    set(ONNX_FILE onnxruntime-win-x64-1.14.1.zip)
    set (ONNX_URL https://github.com/microsoft/onnxruntime/releases/download/v1.14.1/onnxruntime-win-x64-1.14.1.zip)
    download_file(${ONNX_URL}  ${ONNX_FILE})


    include(FaceQAlib/CMakeLists.windows.conan.cmake)
endif (UNIX)

# starting : Tests
include(CTest)
enable_testing()
add_subdirectory(testing)

include(cmake/CPackConfig.cmake)