list(APPEND CMAKE_MODULE_PATH 
	"${CMAKE_SOURCE_DIR}/cmake"
)

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

# Use system-installed OpenCV
find_package(OpenCV REQUIRED COMPONENTS core calib3d imgcodecs imgproc dnn ml)
message(STATUS "Found OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
message(STATUS "OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}")

include_directories (
	${OFIQLIB_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/extern/thirdParty
	${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime-linux-x64-1.17.3/include
	${OpenCV_INCLUDE_DIRS}
	${include_modules}
)

# Include other required dependencies from extern if they exist
list(APPEND OFIQ_LINK_INCLUDE_LIST 
	"${CMAKE_CURRENT_SOURCE_DIR}/extern/flatbuffers/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/extern/json/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/extern/magic_enum/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/extern/di/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/extern/PEGTL/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/extern/abseil-cpp"
)

# Check which directories actually exist and include them
foreach(inc_dir ${OFIQ_LINK_INCLUDE_LIST})
	if(EXISTS ${inc_dir})
		include_directories(${inc_dir})
	endif()
endforeach()

# ONNX Runtime setup
add_library(onnxruntime SHARED IMPORTED)
set_target_properties(onnxruntime PROPERTIES
	IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime-linux-x64-1.17.3/lib/libonnxruntime.so
	IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime-linux-x64-1.17.3/lib/libonnxruntime.so.1.17.3
	INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime-linux-x64-1.17.3/include
)

# Find all source files
add_definitions(-DOFIQ_EXPORTS)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SourceDefinition.cmake)

# Link libraries list
list(APPEND OFIQ_LINK_LIB_LIST
	${OpenCV_LIBS}
	onnxruntime
)

# Build object library
add_library (ofiq_objlib OBJECT
	${module_sources}
	${thirdParty_sources}
	${libImplementationSources})

target_link_libraries(ofiq_objlib
	PRIVATE ${OFIQ_LINK_LIB_LIST}
)

# Build shared library
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