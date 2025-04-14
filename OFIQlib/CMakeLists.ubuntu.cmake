list(APPEND CMAKE_MODULE_PATH 
	"${CMAKE_CURRENT_SOURCE_DIR}/build/conan"
	"${CMAKE_SOURCE_DIR}/cmake"
)
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build/conan")

set(CMAKE_CXX_STANDARD 17)

set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath='$ORIGIN'")
set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath='$ORIGIN'")

# Configure built shared libraries in top-level lib directory
set (OFIQLIB_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/OFIQlib)

# settings for adding information for code coverage using gcov.
option(USE_GCOV_CODECOVERAGE "Enable code coverage using gcov" OFF)


if(USE_GCOV_CODECOVERAGE)
	include(CodeCoverage)
	append_coverage_compiler_flags()

        setup_target_for_coverage_gcovr_xml(
        NAME ctest_coverage                    # New target name
        EXECUTABLE ctest --test-dir madlib/  #-j ${PROCESSOR_COUNT} # Executable in PROJECT_BINARY_DIR
        DEPENDENCIES test_mad                    # Dependencies to build first
        #BASE_DIRECTORY "../"                   # Base directory for report
                                                #  (defaults to PROJECT_SOURCE_DIR)
        #EXCLUDE "src/dir1/*" "src/dir2/*"      # Patterns to exclude (can be relative
                                                #  to BASE_DIRECTORY, with CMake 3.4+)
        )


endif()


# Define local include pathes
file(GLOB_RECURSE include_modules LIST_DIRECTORIES true "${OFIQLIB_SOURCE_DIR}/modules/[^\.]*$")
list(FILTER include_modules EXCLUDE REGEX "/src$")
include_directories (
	${OFIQLIB_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/extern/thirdParty
	${include_modules}
)

if(USE_CONAN)
	# Add packages from conan
	find_package(OpenCV REQUIRED COMPONENTS core calib3d imgcodecs imgproc highgui dnn ml)
	find_package(taocpp-json REQUIRED)
	find_package(magic_enum REQUIRED)

	add_library(onnxruntime SHARED IMPORTED)
	set_target_properties(onnxruntime PROPERTIES
	IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime-linux-x64-1.17.3/lib/libonnxruntime.so
	IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime-linux-x64-1.17.3/lib/libonnxruntime.so.1.17.3
	INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime-linux-x64-1.17.3/include
	)
else(USE_CONAN)
	list(APPEND OFIQ_LINK_INCLUDE_LIST 
		"${CMAKE_CURRENT_SOURCE_DIR}/extern/flatbuffers/include"
		"${CMAKE_CURRENT_SOURCE_DIR}/extern/json/include"
		"${CMAKE_CURRENT_SOURCE_DIR}/extern/magic_enum/include/magic_enum"
		"${CMAKE_CURRENT_SOURCE_DIR}/extern/di/include"
		"${CMAKE_CURRENT_SOURCE_DIR}/extern/PEGTL/include"
		"${CMAKE_CURRENT_SOURCE_DIR}/extern/abseil-cpp"
	)
	include_directories(
        ${OFIQ_LINK_INCLUDE_LIST}
	)
	add_library(onnxruntime SHARED IMPORTED)
	set_target_properties(onnxruntime PROPERTIES
	IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime/build/Linux/${CMAKE_BUILD_TYPE}/libonnxruntime.so
	IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime/build/Linux/${CMAKE_BUILD_TYPE}/libonnxruntime.so.1.18.0
	INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime/include/onnxruntime/core/session
	)
	add_library(IlmImf STATIC IMPORTED)
	set_target_properties(IlmImf PROPERTIES
		IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/lib/opencv4/3rdparty/libIlmImf.a
	)
	add_library(ittnotify STATIC IMPORTED)
	set_target_properties(ittnotify PROPERTIES
		IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/lib/opencv4/3rdparty/libittnotify.a
	)
	add_library(libjpeg-turbo STATIC IMPORTED)
	set_target_properties(libjpeg-turbo PROPERTIES
		IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/lib/opencv4/3rdparty/liblibjpeg-turbo.a
	)
	add_library(libopenjp2 STATIC IMPORTED)
	set_target_properties(libopenjp2 PROPERTIES
		IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/lib/opencv4/3rdparty/liblibopenjp2.a
	)
	add_library(libprotobuf STATIC IMPORTED)
	set_target_properties(libprotobuf PROPERTIES
		IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/lib/opencv4/3rdparty/liblibprotobuf.a
	)
	add_library(OpenCV::core STATIC IMPORTED)
	set_target_properties(OpenCV::core PROPERTIES
		IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/lib/libopencv_core.a
		INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/include/opencv4
	)
	add_library(OpenCV::calib3d STATIC IMPORTED)
	set_target_properties(OpenCV::calib3d PROPERTIES
		IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/lib/libopencv_calib3d.a
		INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/include/opencv4
	)
	add_library(OpenCV::imgcodecs STATIC IMPORTED)
	set_target_properties(OpenCV::imgcodecs PROPERTIES
		IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/lib/libopencv_imgcodecs.a
		INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/include/opencv4
	)
	add_library(OpenCV::imgproc STATIC IMPORTED)
	set_target_properties(OpenCV::imgproc PROPERTIES
		IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/lib/libopencv_imgproc.a
		INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/include/opencv4
	)
	add_library(OpenCV::highgui STATIC IMPORTED)
	set_target_properties(OpenCV::highgui PROPERTIES
		IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/lib/libopencv_highgui.a
		INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/include/opencv4
	)
	add_library(OpenCV::dnn STATIC IMPORTED)
	set_target_properties(OpenCV::dnn PROPERTIES
		IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/lib/libopencv_dnn.a
		INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/include/opencv4
	)
	add_library(OpenCV::ml STATIC IMPORTED)
	set_target_properties(OpenCV::ml PROPERTIES
		IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/lib/libopencv_ml.a
		INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/include/opencv4
	)
	add_library(OpenCV::features2d STATIC IMPORTED)
	set_target_properties(OpenCV::features2d PROPERTIES
		IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/lib/libopencv_features2d.a
		INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/include/opencv4
	)
	add_library(OpenCV::flann STATIC IMPORTED)
	set_target_properties(OpenCV::flann PROPERTIES
		IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/lib/libopencv_flann.a
		INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/include/opencv4
	)

	add_library(OpenCV INTERFACE)
	target_link_libraries(OpenCV INTERFACE OpenCV::calib3d OpenCV::imgcodecs OpenCV::highgui
		OpenCV::dnn OpenCV::ml OpenCV::features2d OpenCV::flann OpenCV::imgproc OpenCV::core IlmImf ittnotify 
		libjpeg-turbo libopenjp2 libprotobuf ${CMAKE_DL_LIBS} -lpthread -lz -lpng)
endif(USE_CONAN)

# Find all source files

add_definitions(-DOFIQ_EXPORTS)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SourceDefinition.cmake)

if(USE_CONAN)
	list(APPEND OFIQ_LINK_LIB_LIST 
		opencv::opencv
		taocpp::json
		magic_enum::magic_enum
		onnxruntime
	)
else(USE_CONAN)
	list(APPEND OFIQ_LINK_LIB_LIST
		onnxruntime
		OpenCV
	)
endif(USE_CONAN)

add_library (ofiq_objlib OBJECT
	${module_sources}
	${thirdParty_sources}
	${libImplementationSources})



target_link_libraries(ofiq_objlib
	PRIVATE ${OFIQ_LINK_LIB_LIST}
	)


add_library(ofiq_lib SHARED $<TARGET_OBJECTS:ofiq_objlib>)

target_link_libraries(ofiq_lib
	PRIVATE ${OFIQ_LINK_LIB_LIST}
	)

# add a test application
add_executable(OFIQSampleApp ${OFIQLIB_SOURCE_DIR}/src/OFIQSampleApp.cpp)
target_link_libraries(OFIQSampleApp
	PRIVATE ofiq_lib
	PRIVATE ${OFIQ_LINK_LIB_LIST}
)


set_target_properties(ofiq_lib 
        PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADER_LIST}"
        )

MESSAGE( STATUS "INSTALLING TARGETS ...")

get_property(IMPORTED_LIB_LOCATION TARGET onnxruntime PROPERTY IMPORTED_LOCATION)

install(FILES "${IMPORTED_LIB_LOCATION}" CONFIGURATIONS Release DESTINATION Release/bin)
install(FILES "${IMPORTED_LIB_LOCATION}" CONFIGURATIONS Release DESTINATION Release/lib)

install(FILES "${IMPORTED_LIB_LOCATION}" CONFIGURATIONS Debug DESTINATION Debug/bin)
install(FILES "${IMPORTED_LIB_LOCATION}" CONFIGURATIONS Debug DESTINATION Debug/lib)

install(TARGETS OFIQSampleApp
	CONFIGURATIONS Release
	DESTINATION Release/bin
)

install(TARGETS ofiq_lib
	CONFIGURATIONS Release
    DESTINATION Release/lib
    PUBLIC_HEADER DESTINATION include/
)

install(TARGETS ofiq_lib 
	CONFIGURATIONS Release
	DESTINATION Release/bin
	PUBLIC_HEADER DESTINATION include/
)

install(TARGETS OFIQSampleApp
	CONFIGURATIONS Debug
	DESTINATION Debug/bin
)

install(TARGETS ofiq_lib
	CONFIGURATIONS Debug
    DESTINATION Debug/lib
    PUBLIC_HEADER DESTINATION include/
)
install(TARGETS ofiq_lib
	CONFIGURATIONS Debug
    DESTINATION Debug/bin
	PUBLIC_HEADER DESTINATION include/
)

install(FILES "$<TARGET_FILE_DIR:ofiq_lib>/ofiq_lib.pdb" DESTINATION "Debug/bin" OPTIONAL)
