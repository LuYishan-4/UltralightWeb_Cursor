set(CMAKE_AUTOMOC ON)


add_executable(
    cursor-ani

    main.cpp
    lib/CursorMain.cpp
    lib/MouseUpload.cpp
    lib/UltralightEffect.cpp
    lib/WaylandBack.cpp
    lib/UserConfig.cpp
)


target_include_directories(
    cursor-ani

    PRIVATE

    ${CMAKE_CURRENT_SOURCE_DIR}/lib/header
)


find_package(Qt6 REQUIRED
    COMPONENTS
        Core
        DBus
        Widgets
)


find_package(X11 REQUIRED)


find_package(Threads REQUIRED)


target_link_libraries(
    cursor-ani

    PRIVATE

    wayland-client

    Qt6::Core
    Qt6::DBus
    Qt6::Widgets

    Ultralight::SDK

    ${X11_LIBRARIES}
    Threads::Threads
)


target_include_directories(
    cursor-ani

    PRIVATE

    ${X11_INCLUDE_DIR}
)