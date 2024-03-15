@echo off

@REM  switch to compiler.version 17 based on parameter provided,
set "set_compiler="
set vs_gen="Visual Studio 16 2019"
IF not "%1%"=="" (
IF "%1%"=="17" (
  set vs_gen="Visual Studio 17 2022"
  set set_compiler=-s:a compiler.version=193
  )
)

erase /Q ..\build\conan\*.*

@REM debug build
conan install ../conan/conanfile.txt --build missing ^
 --profile:build ../conan/conan_profile_debug.txt ^
 --profile:host ../conan/conan_profile_debug.txt ^
  %set_compiler% ^
 --output-folder=../build/conan || goto end

set build_dir=build\build_win
set platform_name=x64  
set install_dir=install_x86_64

pushd %cd%

echo Attempting to build the real implementation

cd ../
if exist %build_dir% (
  echo Removing %build_dir%
  rmdir /S /Q %build_dir% && mkdir %build_dir%
)

echo Generating build files
cmake -S ./ -G %vs_gen% -A %platform_name% -B %build_dir% -DCMAKE_INSTALL_PREFIX=%install_dir% -DDOWNLOAD_MODELS=ON -DDOWNLOAD_IMAGES=ON || goto end

echo Building Debug
cmake --build %build_dir% --config Debug --target install -j 8|| goto end

echo Building finished

@REM pause
popd
exit /B 0

:end
@REM pause
popd
exit /B 1
