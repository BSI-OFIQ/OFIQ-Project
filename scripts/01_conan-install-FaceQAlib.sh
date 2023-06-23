#export CONAN_USER_HOME=~/shared/conan_cache

rm -rf ../build/conan

export FACEQALIB_CONAN_DIR=../conan

conan install ${FACEQALIB_CONAN_DIR}/conanfile.txt \
        --build missing \
        --profile:build ${FACEQALIB_CONAN_DIR}/conan_profile_debug_linux.txt \
        --profile:host ${FACEQALIB_CONAN_DIR}/conan_profile_debug_linux.txt \
        --install-folder=../build/conan \
        -g CMakeDeps \
        -g CMakeToolchain \
        -g cmake_find_package

conan install ${FACEQALIB_CONAN_DIR}/conanfile.txt \
        --build missing \
        --profile:build ${FACEQALIB_CONAN_DIR}/conan_profile_release_linux.txt \
        --profile:host ${FACEQALIB_CONAN_DIR}/conan_profile_release_linux.txt \
        --install-folder=../build/conan \
        -g CMakeDeps \
        -g CMakeToolchain \
        -g cmake_find_package
