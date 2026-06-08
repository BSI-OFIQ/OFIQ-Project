@echo off
set config=Release
IF "%1" == "--debug" (
    set config=Debug
)

pushd %cd%

cd ../build/build_win/Testing
if not exist "..\reports" mkdir "..\reports"

call "%config%/test_config_defaults_jaxn.exe" --gtest_output=xml:"../reports/test_config_defaults_jaxn.xml"
if errorlevel 1 goto :fail

call "%config%/test_conformance_table_jaxnconfig.exe" --gtest_output=xml:"../reports/test_conformance_table_jaxnconfig.xml"
if errorlevel 1 goto :fail

call "%config%/test_conformance_table_inmemoryconfig.exe" --gtest_output=xml:"../reports/test_conformance_table_inmemoryconfig.xml"
if errorlevel 1 goto :fail

popd
exit /b 0

:fail
popd
exit /b 1
