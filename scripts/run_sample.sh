
config_dir=../data
input_image=../data/images/c-07-twofaces.png
config=../install_x86_64_linux/Release

export LD_LIBRARY_PATH=$PWD/$config/lib

./$config/bin/faceqalib_sample_prog -c $config_dir -i $input_image