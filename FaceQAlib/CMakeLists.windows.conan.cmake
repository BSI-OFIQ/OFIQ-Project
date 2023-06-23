message(STATUS  "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build/conan")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build/conan")

set(CMAKE_CXX_STANDARD 17)

# Configure built shared libraries in top-level lib directory
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

set (FACEQALIB_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/FaceQAlib)

# Define local include pathes
file(GLOB_RECURSE include_modules LIST_DIRECTORIES true "${FACEQALIB_SOURCE_DIR}/modules/[^\.]*$")
list(FILTER include_modules EXCLUDE REGEX "/src$")
include_directories (
	${FACEQALIB_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/extern/thirdParty
	${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime-win-x64-1.14.1/include
	${include_modules}
)

# Add packages from conan
find_package(opencv REQUIRED)
find_package(spdlog REQUIRED)
find_package(taocpp-json REQUIRED)
find_package(magic_enum REQUIRED)
find_package(di REQUIRED)

# Find all source files

add_definitions(-DFACEQA_EXPORTS)

list(APPEND PUBLIC_HEADER_LIST 
	${FACEQALIB_SOURCE_DIR}/include/faceqa_lib.h
	${FACEQALIB_SOURCE_DIR}/include/faceqa_structs.h
)

list(APPEND libImplementationSources 
	${FACEQALIB_SOURCE_DIR}/src/FaceQAImpl.cpp
	${FACEQALIB_SOURCE_DIR}/src/FaceQAImplInjectors.cpp
)

list(APPEND module_sources 
	${FACEQALIB_SOURCE_DIR}/modules/detectors/src/detectors.cpp
	${FACEQALIB_SOURCE_DIR}/modules/detectors/src/opencv_ssd_face_detector.cpp
	${FACEQALIB_SOURCE_DIR}/modules/landmarks/src/adnet_landmarks.cpp
	${FACEQALIB_SOURCE_DIR}/modules/landmarks/src/FaceMeasures.cpp
	${FACEQALIB_SOURCE_DIR}/modules/landmarks/src/landmarks.cpp
	${FACEQALIB_SOURCE_DIR}/modules/landmarks/src/PartExtractor.cpp
	${FACEQALIB_SOURCE_DIR}/modules/measures/src/DynamicRange.cpp
	${FACEQALIB_SOURCE_DIR}/modules/measures/src/Executor.cpp
	${FACEQALIB_SOURCE_DIR}/modules/measures/src/Measure.cpp
	${FACEQALIB_SOURCE_DIR}/modules/measures/src/MouthOpen.cpp
	${FACEQALIB_SOURCE_DIR}/modules/measures/src/TotalFacesPresent.cpp
	${FACEQALIB_SOURCE_DIR}/modules/utils/src/Configuration.cpp
	${FACEQALIB_SOURCE_DIR}/modules/utils/src/FaceQaError.cpp
	${FACEQALIB_SOURCE_DIR}/modules/utils/src/image_io.cpp
	${FACEQALIB_SOURCE_DIR}/modules/utils/src/Session.cpp
	${FACEQALIB_SOURCE_DIR}/modules/utils/src/utils.cpp
)


list(APPEND FACEQA_LINK_LIB_LIST 
opencv::opencv_core
opencv::opencv_calib3d
opencv::opencv_imgcodecs
opencv::opencv_highgui
opencv::opencv_imgproc
opencv::opencv_dnn
opencv::opencv_ml
spdlog::spdlog
taocpp::json
magic_enum::magic_enum
di::di
onnxruntime
# ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime-win-x64-1.14.1/lib/onnxruntime.lib
)


add_library(onnxruntime SHARED IMPORTED)
set_target_properties(onnxruntime PROPERTIES
IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime-win-x64-1.14.1/lib/onnxruntime.lib
IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime-win-x64-1.14.1/lib/onnxruntime.dll
INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime-win-x64-1.14.1/include
)



add_library (faceqa_objlib OBJECT
	${module_sources}
	${thirdParty_sources}
	${libImplementationSources})



target_link_libraries(faceqa_objlib
	PRIVATE ${FACEQA_LINK_LIB_LIST}
	)


add_library(faceqa_lib SHARED $<TARGET_OBJECTS:faceqa_objlib>)

target_link_libraries(faceqa_lib
	PRIVATE ${FACEQA_LINK_LIB_LIST}
	)


# Ignore Linker warning for missing PDB Files
set_target_properties(faceqa_lib PROPERTIES LINK_FLAGS "/ignore:4099")

# add a test application
add_executable(faceqalib_sample_prog ${FACEQALIB_SOURCE_DIR}/src/faceqalib_sample_prog.cpp)
target_link_libraries(faceqalib_sample_prog 
	PRIVATE faceqa_lib
	PRIVATE magic_enum::magic_enum
)




# ###
set_target_properties(faceqa_lib 
        PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADER_LIST}"
        )

MESSAGE( STATUS "INSTALLING TARGETS ...")


install(TARGETS faceqalib_sample_prog
        RUNTIME DESTINATION $<CONFIG>/bin
)


get_property(IMPORTED_LIB_LOCATION TARGET onnxruntime PROPERTY IMPORTED_LOCATION)
install(FILES "${IMPORTED_LIB_LOCATION}" DESTINATION "$<CONFIG>/bin")

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/scripts/run_sample.cmd" DESTINATION ".")

install(TARGETS faceqa_lib
        RUNTIME DESTINATION $<CONFIG>/bin
        ARCHIVE DESTINATION $<CONFIG>/lib
        PUBLIC_HEADER DESTINATION $<CONFIG>/include/
)

install(FILES "$<TARGET_FILE_DIR:faceqa_lib>/faceqa_lib.pdb" DESTINATION "$<CONFIG>/bin" OPTIONAL)



