@echo off

pushd %cd%

cd ../build/build_win/Testing
call "Debug/test_conformance_table.exe" --gtest_output=xml:"../reports/%%t.xml"

popd