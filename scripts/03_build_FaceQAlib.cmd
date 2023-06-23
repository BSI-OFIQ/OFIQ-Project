@echo off

set build_dir=../build/build_win

pushd %cd%

echo Attempting to build the real implementation

cd ../FaceQAlib
if not exist "%build_dir%" (
  echo Missing build dir: "%build_dir%"
  goto end
)


@REM echo Building Debug
@REM cmake --build %build_dir% --config Debug --target install  -j 8 || goto end

echo Building Release
cmake --build %build_dir% --config Release --target install -j 8|| goto end


echo Building finished

:end
popd