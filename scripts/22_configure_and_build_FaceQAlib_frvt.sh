
build_dir=../build/build
install_dir=install_x86_64

echo "Attempting to build the real implementation"

cd ../FaceQAlib
echo Removing $build_dir
echo rm -r $build_dir

echo "Generating build files"
cmake -S ./ -B $build_dir

echo "Building Debug"
cmake --build $build_dir --config Debug

echo "Building Release"
cmake --build $build_dir --config Release

echo "Building finished"
