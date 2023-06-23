@echo off

pushd %cd%

cd ../build/build_win
@REM ctest --rerun-failed --output-on-failure

ctest  --output-on-failure

popd