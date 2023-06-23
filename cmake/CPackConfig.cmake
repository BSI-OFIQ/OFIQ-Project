set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
# these are cache variables, so they could be overwritten with -D,
set(
    CPACK_PACKAGE_NAME ${PROJECT_NAME}
    CACHE STRING "The resulting package name"
)
# which is useful in case of packing only selected components instead of the whole thing
set(
    CPACK_PACKAGE_DESCRIPTION_SUMMARY "Face quality analysis application"
    CACHE STRING "Package description for the package metadata"
)
set(CPACK_PACKAGE_VENDOR "Some Company")

set(CPACK_VERBATIM_VARIABLES YES)

set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})


if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CPACK_GENERATOR WIX)
endif()

include(CPack)