@echo off

set build_dir=build\build_win
set architecture=x64  
set install_dir=install_x86_64
set set_compiler=""
set generator="Visual Studio 16 2019"
set use_conan=ON
set download=ON
set config=Release

:loop 
if "%1" == "" goto done
if /i "%1" == "--no-download" set download=OFF
if /i "%1" == "--no-conan" set use_conan=OFF
if /i "%1" == "--arch" set "architecture=%2"
if /i "%1" == "--compiler" set "set_compiler=%2"
if /i "%1" == "--debug" set config=Debug
shift
goto :loop
:done

@REM switch architecture
IF %architecture%==x86 (
  set conan_set_arch=-s:a arch=x86
  set architecture=Win32
  set install_dir=install_x86
  set onnxruntime_flags=--x86
) else (
  set "conan_set_arch="
  set "onnxruntime_flags="
)
set set_architecture=-A %architecture%
@REM  switch to compiler.version 17 based on parameter provided
IF %set_compiler%==17 (
  set generator="Visual Studio 17 2022"
  set compiler=-s:a compiler.version=193
) else if %set_compiler%==gcc (
  set generator="MinGW Makefiles"
  set compiler=-s:a compiler=gcc -s:a compiler.libcxx=libstdc++11 -s:a compiler.version=9
  set "set_architecture="

  echo Building on Windows with gcc is not supported
  goto :end
) else (
  set "compiler="
)

@REM build dependencies
if %use_conan%==ON (
  erase /Q ..\build\conan\*.*
  if %config%==Release (
    conan install ../conan/conanfile.txt --build missing ^
    --profile:build ../conan/conan_profile_release.txt ^
    --profile:host ../conan/conan_profile_release.txt ^
      %compiler% %conan_set_arch% ^
    --output-folder=../build/conan || goto end
  ) else (
    conan install ../conan/conanfile.txt --build missing ^
    --profile:build ../conan/conan_profile_debug.txt ^
    --profile:host ../conan/conan_profile_debug.txt ^
      %compiler% %conan_set_arch% ^
    --output-folder=../build/conan || goto end
  )
) else (
  echo This function is not yet supported
  echo In the future, we may activate building OFIQ from source without conan
  goto :end
  
  echo Building dependencies from source for %config% mode
  @REM build opencv
  cd ../extern/opencv-4.5.5 || goto end
  cmake -S ./ -G %generator% %set_architecture% -B build -DBUILD_LIST=core,calib3d,imgcodecs,highgui,improc,dnn,ml -DBUILD_opencv_java=OFF -DBUILD_opencv_python=OFF -DWITH_FFMPEG=OFF ^
  -DWITH_TIFF=OFF -DWITH_WEBP=OFF -DWITH_IPP=OFF -DWITH_GTK=OFF -DWITH_QT=OFF -DCMAKE_INSTALL_PREFIX=./build/install || goto end
  cmake --build build --config %config% --target install -j 8 || goto end
  @REM build gtest
  cd ../googletest || goto end
  cmake -S ./ -G %generator% %set_architecture% -B build -DBUILD_GMOCK=OFF -DINSTALL_GTEST=OFF -DBUILD_SHARED_LIBS=ON || goto end
  cmake --build build/googletest --config %config% || goto end
  @REM build onnxruntime
  cd ../onnxruntime || goto end
  call build.bat --config %config% --build_shared_lib --parallel --compile_no_warning_as_error --skip_submodule_sync --cmake_generator "Visual Studio 17 2022" --update --build %onnxruntime_flags% || goto end
  cd ..
)
pushd %cd%

echo Attempting to build the real implementation

cd ../
if exist %build_dir% (
  echo Removing %build_dir%
  rmdir /S /Q %build_dir% && mkdir %build_dir%
)

echo Generating build files
cmake -S ./ -G %generator% %set_architecture% -B %build_dir% -DCMAKE_INSTALL_PREFIX=%install_dir% -DDOWNLOAD_ONNX=%use_conan% -DUSE_CONAN=%use_conan% ^
  -DOS=windows -DCMAKE_BUILD_TYPE=%config% -DARCHITECTURE=%architecture% -DDOWNLOAD_MODELS_AND_IMAGES=%download% || goto end

echo Building %config%
cmake --build %build_dir% --config %config% --target install -j 8 || goto end

echo Building finished

@REM pause
popd
exit /B 0

:end
echo Building failed
popd
exit /B 1
