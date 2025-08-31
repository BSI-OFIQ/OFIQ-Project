list(APPEND CMAKE_MODULE_PATH 
	"${CMAKE_SOURCE_DIR}/cmake"
)

set(CMAKE_CXX_STANDARD 17)

set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath='$ORIGIN'")
set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath='$ORIGIN'")

# Configure built shared libraries in top-level lib directory
set (OFIQLIB_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/OFIQlib)

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
	${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime-linux-aarch64-1.17.3/include
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

# ONNX Runtime setup for ARM64
# Check if we're on Jetson and use GPU version if available
if(EXISTS "/etc/nv_tegra_release" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../onnxruntime-linux-aarch64-gpu-1.18.1/lib/libonnxruntime.so")
	message(STATUS "Using GPU-optimized ONNX Runtime for Jetson")
	add_library(onnxruntime SHARED IMPORTED)
	set_target_properties(onnxruntime PROPERTIES
		IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/../onnxruntime-linux-aarch64-gpu-1.18.1/lib/libonnxruntime.so
		IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../onnxruntime-linux-aarch64-gpu-1.18.1/lib/libonnxruntime.so.1.18.1
		INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/../onnxruntime-linux-aarch64-gpu-1.18.1/include
	)
	# Also import the CUDA provider library
	add_library(onnxruntime_providers_cuda SHARED IMPORTED)
	set_target_properties(onnxruntime_providers_cuda PROPERTIES
		IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../onnxruntime-linux-aarch64-gpu-1.18.1/lib/libonnxruntime_providers_cuda.so
	)
	add_library(onnxruntime_providers_shared SHARED IMPORTED)
	set_target_properties(onnxruntime_providers_shared PROPERTIES
		IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../onnxruntime-linux-aarch64-gpu-1.18.1/lib/libonnxruntime_providers_shared.so
	)
else()
	message(STATUS "Using standard ONNX Runtime")
	add_library(onnxruntime SHARED IMPORTED)
	set_target_properties(onnxruntime PROPERTIES
		IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime-linux-aarch64-1.17.3/lib/libonnxruntime.so
		IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime-linux-aarch64-1.17.3/lib/libonnxruntime.so.1.17.3
		INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/extern/onnxruntime-linux-aarch64-1.17.3/include
	)
endif()

# Find all source files
add_definitions(-DOFIQ_EXPORTS)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SourceDefinition.cmake)

# Link libraries list
list(APPEND OFIQ_LINK_LIB_LIST
	${OpenCV_LIBS}
	onnxruntime
)

# Add CUDA providers if using GPU ONNX Runtime
if(EXISTS "/etc/nv_tegra_release" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../onnxruntime-linux-aarch64-gpu-1.18.1/lib/libonnxruntime.so")
	list(APPEND OFIQ_LINK_LIB_LIST
		onnxruntime_providers_cuda
		onnxruntime_providers_shared
	)
endif()

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

# Install CUDA provider libraries if using GPU ONNX Runtime
if(EXISTS "/etc/nv_tegra_release" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../onnxruntime-linux-aarch64-gpu-1.18.1/lib/libonnxruntime.so")
	get_property(CUDA_PROVIDER_LOCATION TARGET onnxruntime_providers_cuda PROPERTY IMPORTED_LOCATION)
	get_property(SHARED_PROVIDER_LOCATION TARGET onnxruntime_providers_shared PROPERTY IMPORTED_LOCATION)
	
	install(FILES "${CUDA_PROVIDER_LOCATION}" CONFIGURATIONS Release DESTINATION Release/bin)
	install(FILES "${CUDA_PROVIDER_LOCATION}" CONFIGURATIONS Release DESTINATION Release/lib)
	install(FILES "${SHARED_PROVIDER_LOCATION}" CONFIGURATIONS Release DESTINATION Release/bin)
	install(FILES "${SHARED_PROVIDER_LOCATION}" CONFIGURATIONS Release DESTINATION Release/lib)
	
	install(FILES "${CUDA_PROVIDER_LOCATION}" CONFIGURATIONS Debug DESTINATION Debug/bin)
	install(FILES "${CUDA_PROVIDER_LOCATION}" CONFIGURATIONS Debug DESTINATION Debug/lib)
	install(FILES "${SHARED_PROVIDER_LOCATION}" CONFIGURATIONS Debug DESTINATION Debug/bin)
	install(FILES "${SHARED_PROVIDER_LOCATION}" CONFIGURATIONS Debug DESTINATION Debug/lib)
endif()

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