message(STATUS  "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_CXX_STANDARD 17)

# Configure built shared libraries in top-level lib directory
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

set (OFIQLIB_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/OFIQlib)

# Define local include pathes
file(GLOB_RECURSE include_modules LIST_DIRECTORIES true "${OFIQLIB_SOURCE_DIR}/modules/[^\.]*$")
list(FILTER include_modules EXCLUDE REGEX "/src$")
include_directories (
	${OFIQLIB_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/extern/thirdParty
	${include_modules}
)

if(USE_CONAN)
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build/conan")
	list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build/conan")


	# Add packages from conan
	find_package(OpenCV REQUIRED COMPONENTS core calib3d imgcodecs imgproc highgui dnn ml)
	find_package(taocpp-json REQUIRED)
	find_package(magic_enum REQUIRED)

	add_library(onnxruntime SHARED IMPORTED)
	if( ARCHITECTURE STREQUAL "x64" )
		set_target_properties(onnxruntime PROPERTIES
		IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime-win-x64-1.17.3/lib/onnxruntime.lib
		IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime-win-x64-1.17.3/lib/onnxruntime.dll
		INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime-win-x64-1.17.3/include
		)
	else ()
		set_target_properties(onnxruntime PROPERTIES
		IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime-win-x86-1.17.3/lib/onnxruntime.lib
		IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime-win-x86-1.17.3/lib/onnxruntime.dll
		INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime-win-x86-1.17.3/include
		)
	endif()
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
	IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime/build/Windows/${CMAKE_BUILD_TYPE}/${CMAKE_BUILD_TYPE}/onnxruntime.lib
	IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime/build/Windows/${CMAKE_BUILD_TYPE}/${CMAKE_BUILD_TYPE}/onnxruntime.dll
	INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime/include/onnxruntime/core/session
	)
	if (VS_VERSION STREQUAL vc16)
		SET(OPENCV_INSTALL_PATH ${ARCHITECTURE}/${VS_VERSION}/)
	endif()
	if (CMAKE_BUILD_TYPE STREQUAL Debug)
		SET(DEBUG_POSTFIX d)
	endif()
	add_library(IlmImf STATIC IMPORTED)
	set_target_properties(IlmImf PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/${OPENCV_INSTALL_PATH}staticlib/IlmImf${DEBUG_POSTFIX}.lib"
	)
	add_library(ittnotify STATIC IMPORTED)
	set_target_properties(ittnotify PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/${OPENCV_INSTALL_PATH}staticlib/ittnotify${DEBUG_POSTFIX}.lib"
	)
	add_library(libjpeg-turbo STATIC IMPORTED)
	set_target_properties(libjpeg-turbo PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/${OPENCV_INSTALL_PATH}staticlib/libjpeg-turbo${DEBUG_POSTFIX}.lib"
	)
	add_library(libopenjp2 STATIC IMPORTED)
	set_target_properties(libopenjp2 PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/${OPENCV_INSTALL_PATH}staticlib/libopenjp2${DEBUG_POSTFIX}.lib"
	)
	add_library(libpng STATIC IMPORTED)
	set_target_properties(libpng PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/${OPENCV_INSTALL_PATH}staticlib/libpng${DEBUG_POSTFIX}.lib"
	)
	add_library(libprotobuf STATIC IMPORTED)
	set_target_properties(libprotobuf PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/${OPENCV_INSTALL_PATH}staticlib/libprotobuf${DEBUG_POSTFIX}.lib"
	)
	add_library(zlib STATIC IMPORTED)
	set_target_properties(zlib PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/${OPENCV_INSTALL_PATH}staticlib/zlib${DEBUG_POSTFIX}.lib"
	)
	add_library(OpenCV::core STATIC IMPORTED)
	set_target_properties(OpenCV::core PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/${OPENCV_INSTALL_PATH}staticlib/opencv_core455${DEBUG_POSTFIX}.lib"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/include"
	)
	add_library(OpenCV::calib3d STATIC IMPORTED)
	set_target_properties(OpenCV::calib3d PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/${OPENCV_INSTALL_PATH}staticlib/opencv_calib3d455${DEBUG_POSTFIX}.lib"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/include"
	)
	add_library(OpenCV::imgcodecs STATIC IMPORTED)
	set_target_properties(OpenCV::imgcodecs PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/${OPENCV_INSTALL_PATH}staticlib/opencv_imgcodecs455${DEBUG_POSTFIX}.lib"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/include"
	)
	add_library(OpenCV::imgproc STATIC IMPORTED)
	set_target_properties(OpenCV::imgproc PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/${OPENCV_INSTALL_PATH}staticlib/opencv_imgproc455${DEBUG_POSTFIX}.lib"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/include"
	)
	add_library(OpenCV::highgui STATIC IMPORTED)
	set_target_properties(OpenCV::highgui PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/${OPENCV_INSTALL_PATH}staticlib/opencv_highgui455${DEBUG_POSTFIX}.lib"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/include"
	)
	add_library(OpenCV::dnn STATIC IMPORTED)
	set_target_properties(OpenCV::dnn PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/${OPENCV_INSTALL_PATH}staticlib/opencv_dnn455${DEBUG_POSTFIX}.lib"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/include"
	)
	add_library(OpenCV::ml STATIC IMPORTED)
	set_target_properties(OpenCV::ml PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/${OPENCV_INSTALL_PATH}staticlib/opencv_ml455${DEBUG_POSTFIX}.lib"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/include"
	)
	add_library(OpenCV::features2d STATIC IMPORTED)
	set_target_properties(OpenCV::features2d PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/${OPENCV_INSTALL_PATH}staticlib/opencv_features2d455${DEBUG_POSTFIX}.lib"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/include"
	)
	add_library(OpenCV::flann STATIC IMPORTED)
	set_target_properties(OpenCV::flann PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/${OPENCV_INSTALL_PATH}staticlib/opencv_flann455${DEBUG_POSTFIX}.lib"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/include"
	)
endif(USE_CONAN)

# Find all source files
add_definitions(-DOFIQ_EXPORTS)

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
		IlmImf
		ittnotify
		libjpeg-turbo
		libopenjp2
		libpng
		libprotobuf
		zlib
		OpenCV::core
		OpenCV::calib3d
		OpenCV::imgcodecs
		OpenCV::imgproc
		OpenCV::highgui
		OpenCV::dnn
		OpenCV::ml
		OpenCV::features2d
		OpenCV::flann
		onnxruntime)
endif(USE_CONAN)


add_library (ofiq_objlib OBJECT
	${module_sources}
	${module_headers}
	${thirdParty_sources}
	${libImplementationSources})



target_link_libraries(ofiq_objlib
	PRIVATE ${OFIQ_LINK_LIB_LIST}
	)


add_library(ofiq_lib SHARED $<TARGET_OBJECTS:ofiq_objlib>)

target_link_libraries(ofiq_lib
	PRIVATE ${OFIQ_LINK_LIB_LIST}
	)


# Ignore Linker warning for missing PDB Files
set_target_properties(ofiq_lib PROPERTIES LINK_FLAGS "/ignore:4099")

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


install(TARGETS OFIQSampleApp
        RUNTIME DESTINATION $<CONFIG>/bin
)


get_property(IMPORTED_LIB_LOCATION TARGET onnxruntime PROPERTY IMPORTED_LOCATION)
install(FILES "${IMPORTED_LIB_LOCATION}" DESTINATION "$<CONFIG>/bin")

install(TARGETS ofiq_lib
        RUNTIME DESTINATION $<CONFIG>/bin
        ARCHIVE DESTINATION $<CONFIG>/lib
        PUBLIC_HEADER DESTINATION $<CONFIG>/include/
)

install(FILES "$<TARGET_FILE_DIR:ofiq_lib>/ofiq_lib.pdb" DESTINATION "$<CONFIG>/bin" OPTIONAL)