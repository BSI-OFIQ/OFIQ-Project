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

# Define local include pathes
file(GLOB_RECURSE include_modules LIST_DIRECTORIES true "${OFIQLIB_SOURCE_DIR}/modules/[^\.]*$")
list(FILTER include_modules EXCLUDE REGEX "/src$")
include_directories (
	${OFIQLIB_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/extern/thirdParty
	${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime-linux-x64-1.14.1/include
	${include_modules}
)

# Add packages from conan
find_package(LAPACK REQUIRED)
find_package(OpenCV REQUIRED COMPONENTS core calib3d imgcodecs imgproc highgui dnn ml)
find_package(spdlog REQUIRED)
find_package(taocpp-json REQUIRED)
find_package(magic_enum REQUIRED)

# Find all source files

add_definitions(-DOFIQ_EXPORTS)
add_definitions(-DOFIQ_SINGLE_FACE_PRESENT_WITH_TMETRIC)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

list(APPEND PUBLIC_HEADER_LIST 
	${OFIQLIB_SOURCE_DIR}/include/ofiq_lib.h
	${OFIQLIB_SOURCE_DIR}/include/ofiq_structs.h
)

list(APPEND libImplementationSources 
	${OFIQLIB_SOURCE_DIR}/src/OFIQImpl.cpp
	${OFIQLIB_SOURCE_DIR}/src/OFIQInitialization.cpp
)

list(APPEND module_sources 
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


list(APPEND OFIQ_LINK_LIB_LIST 
	opencv::opencv
	spdlog::spdlog
	taocpp::json
	magic_enum::magic_enum
	LAPACK::LAPACK
	onnxruntime
)

add_library(onnxruntime SHARED IMPORTED)
set_target_properties(onnxruntime PROPERTIES
IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime-linux-x64-1.14.1/lib/libonnxruntime.so
IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime-linux-x64-1.14.1/lib/libonnxruntime.so.1.14.1
INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime-linux-x64-1.14.1/include
)

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
	PRIVATE magic_enum::magic_enum
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
)

install(FILES "$<TARGET_FILE_DIR:ofiq_lib>/ofiq_lib.pdb" DESTINATION "Debug/bin" OPTIONAL)
