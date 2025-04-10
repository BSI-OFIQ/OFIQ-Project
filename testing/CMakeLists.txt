message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

if(USE_CONAN)
	find_package(GTest REQUIRED)
else(USE_CONAN)
        if(${OS} STREQUAL "windows")
                add_library(GTest::gtest SHARED IMPORTED)
                set_target_properties(GTest::gtest PROPERTIES
                IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/extern/googletest/build/lib/${CMAKE_BUILD_TYPE}/gtest.lib
                IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/extern/googletest/build/bin/${CMAKE_BUILD_TYPE}/gtest.dll
                INTERFACE_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/extern/googletest/googletest/include
                )

                add_library(GTest::gtest_main SHARED IMPORTED)
                set_target_properties(GTest::gtest_main PROPERTIES
                IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/extern/googletest/build/lib/${CMAKE_BUILD_TYPE}/gtest_main.lib
                IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/extern/googletest/build/bin/${CMAKE_BUILD_TYPE}/gtest_main.dll
                INTERFACE_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/extern/googletest/googletest/include
                )

                list(APPEND OFIQ_LINK_LIB_LIST
                        GTest::gtest
                        GTest::gtest_main
                )
                get_property(IMPORTED_LIB_LOCATION TARGET GTest::gtest PROPERTY IMPORTED_LOCATION)
                install(FILES "${IMPORTED_LIB_LOCATION}" DESTINATION ${CMAKE_BINARY_DIR}/Testing/${CMAKE_BUILD_TYPE}/)
                get_property(IMPORTED_LIB_LOCATION TARGET GTest::gtest_main PROPERTY IMPORTED_LOCATION)
                install(FILES "${IMPORTED_LIB_LOCATION}" DESTINATION ${CMAKE_BINARY_DIR}/Testing/${CMAKE_BUILD_TYPE}/)
        elseif(${OS} STREQUAL "linux")
                add_library(GTest::gtest SHARED IMPORTED)
                set_target_properties(GTest::gtest PROPERTIES
                IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/extern/googletest/build/lib/libgtest.so
                IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/extern/googletest/build/lib/libgtest.so.1.14.0
                INTERFACE_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/extern/googletest/googletest/include
                )

                add_library(GTest::gtest_main SHARED IMPORTED)
                set_target_properties(GTest::gtest_main PROPERTIES
                IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/extern/googletest/build/lib/libgtest_main.so
                IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/extern/googletest/build/lib/libgtest_main.so.1.14.0
                INTERFACE_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/extern/googletest/googletest/include
                )
        elseif(${OS} STREQUAL "macos")
                add_library(GTest::gtest SHARED IMPORTED)
                set_target_properties(GTest::gtest PROPERTIES
                IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/extern/googletest/build/lib/libgtest.dylib
                IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/extern/googletest/build/lib/libgtest.1.14.0.dylib
                INTERFACE_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/extern/googletest/googletest/include
                )

                add_library(GTest::gtest_main SHARED IMPORTED)
                set_target_properties(GTest::gtest_main PROPERTIES
                IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/extern/googletest/build/lib/libgtest_main.dylib
                IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/extern/googletest/build/lib/libgtest_main.1.14.0.dylib
                INTERFACE_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/extern/googletest/googletest/include
                )
        endif(${OS} STREQUAL "windows")
endif(USE_CONAN)

if(${OS} STREQUAL "windows")
        get_property(IMPORTED_LIB_LOCATION TARGET onnxruntime PROPERTY IMPORTED_LOCATION)
        install(FILES "${IMPORTED_LIB_LOCATION}" DESTINATION ${CMAKE_BINARY_DIR}/Testing/${CMAKE_BUILD_TYPE}/)
endif(${OS} STREQUAL "windows")

include(GoogleTest)

# #############
# UNIT TESTS #
# #############
set(TEST_RESULT_DIR reports)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/${TEST_RESULT_DIR})
set(UNIT_TEST_WORKING_DIR ${PROJECT_BINARY_DIR}/${TEST_RESULT_DIR})

set(UNIT_TEST_FILE "test_conformance_table.cpp")

get_filename_component(ut_target ${UNIT_TEST_FILE} NAME_WLE)
add_executable(${ut_target} ${UNIT_TEST_FILE})

target_include_directories( ${ut_target}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${ut_target}
        PRIVATE
        $<TARGET_OBJECTS:ofiq_objlib>
        ${OFIQ_LINK_LIB_LIST}
        GTest::gtest
        GTest::gtest_main
)

gtest_discover_tests(
        ${ut_target}
        TEST_LIST myListOfTests
        XML_OUTPUT_DIR ${CMAKE_BINARY_DIR}/reports
        DISCOVERY_MODE PRE_TEST
)