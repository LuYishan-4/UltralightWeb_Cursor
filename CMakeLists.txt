cmake_minimum_required(VERSION 3.16)

project(cursor-ani)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FeatureSummary)

#
# Ultralight SDK
#
set(ULTRALIGHT_ROOT
    ${CMAKE_SOURCE_DIR}/sdk/ultralight-free-sdk-1.4.0-linux-x64
)

if(NOT EXISTS ${ULTRALIGHT_ROOT}/CMakeLists.txt)
    message(FATAL_ERROR
        "Ultralight SDK not found: ${ULTRALIGHT_ROOT}"
    )
endif()


#
# Build Ultralight as subproject
#
add_subdirectory(
    ${ULTRALIGHT_ROOT}
    ${CMAKE_BINARY_DIR}/ultralight
)


#
# Runtime search path for Ultralight .so
#
set(CMAKE_BUILD_RPATH
    "${ULTRALIGHT_ROOT}/bin"
)

set(CMAKE_INSTALL_RPATH
    "${ULTRALIGHT_ROOT}/bin"
)


#
# Project source
#
add_subdirectory(src)


#
# Summary
#
feature_summary(
    WHAT ALL
    INCLUDE_QUIET_PACKAGES
    FATAL_ON_MISSING_REQUIRED_PACKAGES
)