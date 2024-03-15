message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

find_package(GTest REQUIRED)
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

if(WIN32)
        add_custom_command(TARGET ${ut_target} PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                $<TARGET_RUNTIME_DLLS:${ut_target}>
                $<TARGET_FILE_DIR:${ut_target}>
                COMMAND_EXPAND_LISTS)
endif()
