@echo off
set config=Release
IF "%1" == "--debug" (
    set config=Debug
)

pushd %cd%

cd ../build/build_win/Testing
call "%config%/test_conformance_table.exe" --gtest_output=xml:"../reports/%%t.xml"

popd