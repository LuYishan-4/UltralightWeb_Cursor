cmake_minimum_required(VERSION 3.16)

project(cursor-ani)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FeatureSummary)

#
# Qt6
#
find_package(
    Qt6 REQUIRED
    COMPONENTS
        Core
        Gui
        Widgets
)


set(ULTRALIGHT_ROOT
    ${CMAKE_SOURCE_DIR}/sdk/ultralight-free-sdk-1.4.0-linux-x64
)
set(ULTRALIGHT_BIN 
    "${ULTRALIGHT_ROOT}/bin"
)


if(NOT EXISTS "${ULTRALIGHT_ROOT}/include/AppCore/App.h")
    message(FATAL_ERROR
        "Ultralight SDK not found: ${ULTRALIGHT_ROOT}"
    )
endif()

add_library(Ultralight::Ultralight SHARED IMPORTED)
set_target_properties(Ultralight::Ultralight PROPERTIES
    IMPORTED_LOCATION "${ULTRALIGHT_BIN}/libUltralight.so"
    INTERFACE_INCLUDE_DIRECTORIES "${ULTRALIGHT_ROOT}/include"
)

add_library(Ultralight::UltralightCore SHARED IMPORTED)
set_target_properties(Ultralight::UltralightCore PROPERTIES
    IMPORTED_LOCATION "${ULTRALIGHT_BIN}/libUltralightCore.so"
    INTERFACE_INCLUDE_DIRECTORIES "${ULTRALIGHT_ROOT}/include"
)

add_library(Ultralight::AppCore SHARED IMPORTED)
set_target_properties(Ultralight::AppCore PROPERTIES
    IMPORTED_LOCATION "${ULTRALIGHT_BIN}/libAppCore.so"
    INTERFACE_INCLUDE_DIRECTORIES "${ULTRALIGHT_ROOT}/include"
)

add_library(Ultralight::WebCore SHARED IMPORTED)
set_target_properties(Ultralight::WebCore PROPERTIES
    IMPORTED_LOCATION "${ULTRALIGHT_BIN}/libWebCore.so"
    INTERFACE_INCLUDE_DIRECTORIES "${ULTRALIGHT_ROOT}/include"
)

add_library(Ultralight::SDK INTERFACE IMPORTED)
target_link_libraries(Ultralight::SDK INTERFACE 
    Ultralight::Ultralight
    Ultralight::UltralightCore
    Ultralight::AppCore
    Ultralight::WebCore
)


set(CMAKE_BUILD_RPATH
    "${ULTRALIGHT_ROOT}/bin"
)

set(CMAKE_INSTALL_RPATH
    "${ULTRALIGHT_ROOT}/bin"
)


add_subdirectory(src)



feature_summary(
    WHAT ALL
    INCLUDE_QUIET_PACKAGES
    FATAL_ON_MISSING_REQUIRED_PACKAGES
)