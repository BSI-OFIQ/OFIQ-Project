#export CONAN_USER_HOME=/home/secunet/shared/conan_cache

build_dir=build/build_linux
#platform_name=x64  
install_dir=install_x86_64_linux

echo "Attempting to build the real implementation"

cd ../
echo Removing $build_dir
rm -r $build_dir

echo "Generating build files"
cmake -S ./ -B $build_dir -DCMAKE_INSTALL_PREFIX=$install_dir -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON
# cmake -S ./ -B $build_dir -DCMAKE_INSTALL_PREFIX=$install_dir -DCMAKE_BUILD_TYPE=Debug -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON

cmake --build $build_dir --target install

echo "Building finished"

# cd $build_dir
# ctest
# cpack
