@echo off

set config_dir=..\data
set input_image=..\data\images\c-07-twofaces.png
set config=..\install_x86_64\Release

.\%config%\bin\faceqalib_sample_prog.exe -c %config_dir% -i %input_image%