message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

find_package(GTest REQUIRED)
include(GoogleTest)

# #############
# UNIT TESTS #
# #############
set(TEST_RESULT_DIR reports)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/${TEST_RESULT_DIR})
set(UNIT_TEST_WORKING_DIR ${PROJECT_BINARY_DIR}/${TEST_RESULT_DIR})

list(APPEND UNIT_TESTS_LIST
        "test_config_and_initialization.cpp"
        "test_facedetector.cpp"
        "test_landmarkdetector.cpp"
		"test_total_number_of_faces.cpp"
		"test_dynamic_range.cpp"
		"test_mouth_open.cpp"
)

foreach(ut_file ${UNIT_TESTS_LIST})
        get_filename_component(ut_target ${ut_file} NAME_WLE)
        add_executable(${ut_target} ${ut_file})

        target_link_libraries(${ut_target}
                PRIVATE
                $<TARGET_OBJECTS:faceqa_objlib>
                ${FACEQA_LINK_LIB_LIST}
                GTest::gtest
                GTest::gtest_main
        )

        gtest_discover_tests(
                ${ut_target}
                TEST_LIST myListOfTests
                XML_OUTPUT_DIR ${CMAKE_BINARY_DIR}/reports

                # DISCOVERY_MODE PRE_TEST
        )

        if(WIN32)
                # TODO: only required for windows
                # https://stackoverflow.com/questions/68579934/how-to-copy-targets-all-runtime-dependencies-after-build-process-in-cmake
                add_custom_command(TARGET ${ut_target} PRE_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy
                        $<TARGET_RUNTIME_DLLS:${ut_target}>
                        $<TARGET_FILE_DIR:${ut_target}>
                        COMMAND_EXPAND_LISTS)
        endif()

        # add_custom_command( TARGET ${ut_target} POST_BUILD
        # COMMAND ${CMAKE_COMMAND} -E copy
        # $<TARGET_RUNTIME_DLLS:${ut_target}>
        # $<TARGET_FILE_DIR:${ut_target}>
        # COMMAND_EXPAND_LISTS)
endforeach(ut_file ${UNIT_TESTS_LIST})
