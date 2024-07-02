message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

if(USE_CONAN)
	find_package(GTest REQUIRED)
else(USE_CONAN)
        if(${OS} STREQUAL "Windows")
                add_library(GTest::gtest SHARED IMPORTED)
                set_target_properties(GTest::gtest PROPERTIES
                IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/extern/googletest/build/lib/${BUILD_TYPE}/gtest.lib
                IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/extern/googletest/build/bin/${BUILD_TYPE}/gtest.dll
                INTERFACE_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/extern/googletest/googletest/include
                )

                add_library(GTest::gtest_main SHARED IMPORTED)
                set_target_properties(GTest::gtest_main PROPERTIES
                IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/extern/googletest/build/lib/${BUILD_TYPE}/gtest_main.lib
                IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/extern/googletest/build/bin/${BUILD_TYPE}/gtest_main.dll
                INTERFACE_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/extern/googletest/googletest/include
                )
        else(${OS} STREQUAL "Windows") # Linux
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
        endif(${OS} STREQUAL "Windows")
endif(USE_CONAN)

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

#if(WIN32)
#        add_custom_command(TARGET ${ut_target} PRE_BUILD
#                COMMAND ${CMAKE_COMMAND} -E copy
#                $<TARGET_RUNTIME_DLLS:${ut_target}>
#                $<TARGET_FILE_DIR:${ut_target}>
#                COMMAND_EXPAND_LISTS)
#endif()

if(${OS} STREQUAL "windows")
        get_property(IMPORTED_LIB_LOCATION TARGET onnxruntime PROPERTY IMPORTED_LOCATION)
        install(FILES "${IMPORTED_LIB_LOCATION}" DESTINATION ${CMAKE_BINARY_DIR}/Testing/${CMAKE_BUILD_TYPE}/)
endif(${OS} STREQUAL "windows")