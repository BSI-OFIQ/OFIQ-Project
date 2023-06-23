
config_dir=../data
input_image=../data/images/Emilia03.jpg
config=Release

export LD_LIBRARY_PATH=$PWD/$config/lib

./$config/bin/faceqalib_sample_prog -c $config_dir -i $input_image