@echo off

@REM  switch to visual studio 2022 based on parameter (compiler version) provided,
@REM  required for Jenkins build.
set vs_gen="Visual Studio 16 2019"
IF not "%1%"=="" (
IF "%1%"=="17" (set vs_gen="Visual Studio 17 2022")
)


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
cmake -S ./ -G %vs_gen% -A %platform_name% -B %build_dir% -DCMAKE_INSTALL_PREFIX=%install_dir%  || goto end

@REM echo Building Debug
@REM cmake --build %build_dir% --config Debug --target install  -j 8 || goto end

echo Building Release
cmake --build %build_dir% --config Release --target install -j 8|| goto end

echo Building finished

@REM cd %build_dir%
@REM ctest
@REM cpack


:end
popd