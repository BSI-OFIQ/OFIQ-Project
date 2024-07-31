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
	if (CMAKE_BUILD_TYPE STREQUAL Debug)
		SET(OPENCV_D d)
	endif()
	add_library(OpenCV::core SHARED IMPORTED)
	set_target_properties(OpenCV::core PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/bin/${CMAKE_BUILD_TYPE}/opencv_core455${OPENCV_D}.dll"
		IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/lib/${CMAKE_BUILD_TYPE}/opencv_core455${OPENCV_D}.lib"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/include"
	)
	add_library(OpenCV::calib3d SHARED IMPORTED)
	set_target_properties(OpenCV::calib3d PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/bin/${CMAKE_BUILD_TYPE}/opencv_calib3d455${OPENCV_D}.dll"
		IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/lib/${CMAKE_BUILD_TYPE}/opencv_calib3d455${OPENCV_D}.lib"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/include"
	)
	add_library(OpenCV::imgcodecs SHARED IMPORTED)
	set_target_properties(OpenCV::imgcodecs PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/bin/${CMAKE_BUILD_TYPE}/opencv_imgcodecs455${OPENCV_D}.dll"
		IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/lib/${CMAKE_BUILD_TYPE}/opencv_imgcodecs455${OPENCV_D}.lib"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/include"
	)
	add_library(OpenCV::imgproc SHARED IMPORTED)
	set_target_properties(OpenCV::imgproc PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/bin/${CMAKE_BUILD_TYPE}/opencv_imgproc455${OPENCV_D}.dll"
		IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/lib/${CMAKE_BUILD_TYPE}/opencv_imgproc455${OPENCV_D}.lib"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/include"
	)
	add_library(OpenCV::highgui SHARED IMPORTED)
	set_target_properties(OpenCV::highgui PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/bin/${CMAKE_BUILD_TYPE}/opencv_highgui455${OPENCV_D}.dll"
		IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/lib/${CMAKE_BUILD_TYPE}/opencv_highgui455${OPENCV_D}.lib"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/include"
	)
	add_library(OpenCV::dnn SHARED IMPORTED)
	set_target_properties(OpenCV::dnn PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/bin/${CMAKE_BUILD_TYPE}/opencv_dnn455${OPENCV_D}.dll"
		IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/lib/${CMAKE_BUILD_TYPE}/opencv_dnn455${OPENCV_D}.lib"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/include"
	)
	add_library(OpenCV::ml SHARED IMPORTED)
	set_target_properties(OpenCV::ml PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/bin/${CMAKE_BUILD_TYPE}/opencv_ml455${OPENCV_D}.dll"
		IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/lib/${CMAKE_BUILD_TYPE}/opencv_ml455${OPENCV_D}.lib"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/include"
	)
	add_library(OpenCV::features2d SHARED IMPORTED)
	set_target_properties(OpenCV::features2d PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/bin/${CMAKE_BUILD_TYPE}/opencv_features2d455${OPENCV_D}.dll"
		IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/lib/${CMAKE_BUILD_TYPE}/opencv_features2d455${OPENCV_D}.lib"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/include"
	)
	add_library(OpenCV::flann SHARED IMPORTED)
	set_target_properties(OpenCV::flann PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/bin/${CMAKE_BUILD_TYPE}/opencv_flann455${OPENCV_D}.dll"
		IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/lib/${CMAKE_BUILD_TYPE}/opencv_flann455${OPENCV_D}.lib"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/extern/opencv-4.5.5/build/install/include"
	)
endif(USE_CONAN)

# Find all source files
add_definitions(-DOFIQ_EXPORTS)

list(APPEND PUBLIC_HEADER_LIST 
	${OFIQLIB_SOURCE_DIR}/include/ofiq_lib.h
	${OFIQLIB_SOURCE_DIR}/include/ofiq_structs.h
)

list(APPEND libImplementationSources 
	${OFIQLIB_SOURCE_DIR}/src/OFIQImpl.cpp
	${OFIQLIB_SOURCE_DIR}/src/OFIQInitialization.cpp
)

list(APPEND module_sources 
	${libImplementationSources}
	${OFIQLIB_SOURCE_DIR}/modules/detectors/src/detectors.cpp
	${OFIQLIB_SOURCE_DIR}/modules/detectors/src/opencv_ssd_face_detector.cpp
	${OFIQLIB_SOURCE_DIR}/modules/landmarks/src/adnet_landmarks.cpp
	${OFIQLIB_SOURCE_DIR}/modules/landmarks/src/FaceMeasures.cpp
	${OFIQLIB_SOURCE_DIR}/modules/landmarks/src/landmarks.cpp
	${OFIQLIB_SOURCE_DIR}/modules/landmarks/src/PartExtractor.cpp
	${OFIQLIB_SOURCE_DIR}/modules/measures/src/BackgroundUniformity.cpp
	${OFIQLIB_SOURCE_DIR}/modules/measures/src/CompressionArtifacts.cpp
	${OFIQLIB_SOURCE_DIR}/modules/measures/src/DynamicRange.cpp
	${OFIQLIB_SOURCE_DIR}/modules/measures/src/Executor.cpp
	${OFIQLIB_SOURCE_DIR}/modules/measures/src/ExpressionNeutrality.cpp
	${OFIQLIB_SOURCE_DIR}/modules/measures/src/EyesOpen.cpp
	${OFIQLIB_SOURCE_DIR}/modules/measures/src/EyesVisible.cpp
	${OFIQLIB_SOURCE_DIR}/modules/measures/src/FaceOcclusionPrevention.cpp
	${OFIQLIB_SOURCE_DIR}/modules/measures/src/CropOfTheFaceImage.cpp
	${OFIQLIB_SOURCE_DIR}/modules/measures/src/UnifiedQualityScore.cpp
	${OFIQLIB_SOURCE_DIR}/modules/measures/src/IlluminationUniformity.cpp
	${OFIQLIB_SOURCE_DIR}/modules/measures/src/InterEyeDistance.cpp
	${OFIQLIB_SOURCE_DIR}/modules/measures/src/Luminance.cpp
	${OFIQLIB_SOURCE_DIR}/modules/measures/src/Measure.cpp
	${OFIQLIB_SOURCE_DIR}/modules/measures/src/MeasureFactory.cpp
	${OFIQLIB_SOURCE_DIR}/modules/measures/src/MouthOcclusionPrevention.cpp
	${OFIQLIB_SOURCE_DIR}/modules/measures/src/MouthClosed.cpp
	${OFIQLIB_SOURCE_DIR}/modules/measures/src/NoHeadCoverings.cpp
	${OFIQLIB_SOURCE_DIR}/modules/measures/src/OverExposurePrevention.cpp
	${OFIQLIB_SOURCE_DIR}/modules/measures/src/UnderExposurePrevention.cpp
	${OFIQLIB_SOURCE_DIR}/modules/measures/src/HeadSize.cpp
	${OFIQLIB_SOURCE_DIR}/modules/measures/src/HeadPose.cpp
	${OFIQLIB_SOURCE_DIR}/modules/measures/src/Sharpness.cpp
	${OFIQLIB_SOURCE_DIR}/modules/measures/src/SingleFacePresent.cpp
	${OFIQLIB_SOURCE_DIR}/modules/measures/src/NaturalColour.cpp
	${OFIQLIB_SOURCE_DIR}/modules/poseEstimators/src/HeadPose3DDFAV2.cpp
	${OFIQLIB_SOURCE_DIR}/modules/poseEstimators/src/poseEstimators.cpp
	${OFIQLIB_SOURCE_DIR}/modules/segmentations/src/ONNXRTSegmentation.cpp
	${OFIQLIB_SOURCE_DIR}/modules/segmentations/src/FaceParsing.cpp
	${OFIQLIB_SOURCE_DIR}/modules/segmentations/src/FaceOcclusionSegmentation.cpp
	${OFIQLIB_SOURCE_DIR}/modules/segmentations/src/segmentations.cpp
	${OFIQLIB_SOURCE_DIR}/modules/utils/src/Configuration.cpp
	${OFIQLIB_SOURCE_DIR}/modules/utils/src/OFIQError.cpp
	${OFIQLIB_SOURCE_DIR}/modules/utils/src/image_io.cpp
	${OFIQLIB_SOURCE_DIR}/modules/utils/src/image_utils.cpp
	${OFIQLIB_SOURCE_DIR}/modules/utils/src/Session.cpp
	${OFIQLIB_SOURCE_DIR}/modules/utils/src/utils.cpp
)

list(APPEND module_headers
	${PUBLIC_HEADER_LIST}
	${OFIQLIB_SOURCE_DIR}/include/ofiq_lib_impl.h
	${OFIQLIB_SOURCE_DIR}/modules/detectors/AllDetectors.h
	${OFIQLIB_SOURCE_DIR}/modules/detectors/detectors.h
	${OFIQLIB_SOURCE_DIR}/modules/detectors/opencv_ssd_face_detector.h
	${OFIQLIB_SOURCE_DIR}/modules/landmarks/AllLandmarks.h
	${OFIQLIB_SOURCE_DIR}/modules/landmarks/adnet_landmarks.h
	${OFIQLIB_SOURCE_DIR}/modules/landmarks/adnet_FaceMap.h
	${OFIQLIB_SOURCE_DIR}/modules/landmarks/FaceMeasures.h
	${OFIQLIB_SOURCE_DIR}/modules/landmarks/FaceParts.h
	${OFIQLIB_SOURCE_DIR}/modules/landmarks/landmarks.h
	${OFIQLIB_SOURCE_DIR}/modules/landmarks/PartExtractor.h
	${OFIQLIB_SOURCE_DIR}/modules/measures/AllMeasures.h
	${OFIQLIB_SOURCE_DIR}/modules/measures/BackgroundUniformity.h
	${OFIQLIB_SOURCE_DIR}/modules/measures/CompressionArtifacts.h
	${OFIQLIB_SOURCE_DIR}/modules/measures/DynamicRange.h
	${OFIQLIB_SOURCE_DIR}/modules/measures/Executor.h
	${OFIQLIB_SOURCE_DIR}/modules/measures/ExpressionNeutrality.h
	${OFIQLIB_SOURCE_DIR}/modules/measures/EyesOpen.h
	${OFIQLIB_SOURCE_DIR}/modules/measures/EyesVisible.h
	${OFIQLIB_SOURCE_DIR}/modules/measures/FaceOcclusionPrevention.h
	${OFIQLIB_SOURCE_DIR}/modules/measures/CropOfTheFaceImage.h
	${OFIQLIB_SOURCE_DIR}/modules/measures/UnifiedQualityScore.h
	${OFIQLIB_SOURCE_DIR}/modules/measures/IlluminationUniformity.h
	${OFIQLIB_SOURCE_DIR}/modules/measures/InterEyeDistance.h
	${OFIQLIB_SOURCE_DIR}/modules/measures/Luminance.h
	${OFIQLIB_SOURCE_DIR}/modules/measures/Measure.h
	${OFIQLIB_SOURCE_DIR}/modules/measures/MeasureFactory.h
	${OFIQLIB_SOURCE_DIR}/modules/measures/MouthOcclusionPrevention.h
	${OFIQLIB_SOURCE_DIR}/modules/measures/MouthClosed.h
	${OFIQLIB_SOURCE_DIR}/modules/measures/NoHeadCoverings.h
	${OFIQLIB_SOURCE_DIR}/modules/measures/OverExposurePrevention.h
	${OFIQLIB_SOURCE_DIR}/modules/measures/UnderExposurePrevention.h
	${OFIQLIB_SOURCE_DIR}/modules/measures/HeadSize.h
	${OFIQLIB_SOURCE_DIR}/modules/measures/HeadPose.h
	${OFIQLIB_SOURCE_DIR}/modules/measures/Sharpness.h
	${OFIQLIB_SOURCE_DIR}/modules/measures/SingleFacePresent.h
	${OFIQLIB_SOURCE_DIR}/modules/measures/NaturalColour.h
	${OFIQLIB_SOURCE_DIR}/modules/poseEstimators/AllPoseEstimators.h
	${OFIQLIB_SOURCE_DIR}/modules/poseEstimators/HeadPose3DDFAV2.h
	${OFIQLIB_SOURCE_DIR}/modules/poseEstimators/poseEstimators.h
	${OFIQLIB_SOURCE_DIR}/modules/segmentations/ONNXRTSegmentation.h
	${OFIQLIB_SOURCE_DIR}/modules/segmentations/FaceParsing.h
	${OFIQLIB_SOURCE_DIR}/modules/segmentations/FaceOcclusionSegmentation.h
	${OFIQLIB_SOURCE_DIR}/modules/segmentations/segmentations.h
	${OFIQLIB_SOURCE_DIR}/modules/utils/Configuration.h
	${OFIQLIB_SOURCE_DIR}/modules/utils/OFIQError.h
	${OFIQLIB_SOURCE_DIR}/modules/utils/image_io.h
	${OFIQLIB_SOURCE_DIR}/modules/utils/image_utils.h
	${OFIQLIB_SOURCE_DIR}/modules/utils/NeuronalNetworkContainer.h
	${OFIQLIB_SOURCE_DIR}/modules/utils/Session.h
	${OFIQLIB_SOURCE_DIR}/modules/utils/utils.h
)

if(USE_CONAN)
	list(APPEND OFIQ_LINK_LIB_LIST 
		opencv::opencv
		taocpp::json
		magic_enum::magic_enum
		onnxruntime
	)
else(USE_CONAN)
	list(APPEND OFIQ_LINK_LIB_LIST
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