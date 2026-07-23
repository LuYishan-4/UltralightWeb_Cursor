cmake_minimum_required(VERSION 3.16)

project(UltralightWebCursor-GUI)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    QuickControls2
    DBus
)


qt_add_executable(
    UltralightWebCursor-GUI

    main_settings.cpp
     ${CMAKE_SOURCE_DIR}/src/lib/Quick/PluginPath/PluginPath.cpp
    SettingsBackend.cpp

    ${CMAKE_SOURCE_DIR}/src/lib/UserConfig.cpp
)


set_target_properties(
    UltralightWebCursor-GUI
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY
    "${CMAKE_BINARY_DIR}/bin"
)


target_include_directories(
    UltralightWebCursor-GUI
    PRIVATE

    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/lib/header
)


qt_add_qml_module(
    UltralightWebCursor-GUI

    URI CursorFXSettings
    VERSION 1.0

    QML_FILES
        Main.qml
    RESOURCES
        assets/github.svg
)


target_link_libraries(
    UltralightWebCursor-GUI

    PRIVATE

    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
)


install(
    TARGETS UltralightWebCursor-GUI

    RUNTIME DESTINATION
    "$ENV{HOME}/.local/bin"
)


set(CURSORFX_SETTINGS_BIN
    "$ENV{HOME}/.local/bin/UltralightWebCursor-GUI"
)


configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/UltralightWebCursorGUI_Desktop.in"

    "${CMAKE_CURRENT_BINARY_DIR}/UltralightWebCursor-GUI.desktop"

    @ONLY
)


install(
    FILES
    "${CMAKE_CURRENT_BINARY_DIR}/UltralightWebCursor-GUI.desktop"

    DESTINATION
    "$ENV{HOME}/.local/share/applications"
)