kcoreaddons_add_plugin(
    ultralightcursor
    INSTALL_NAMESPACE "kwin/effects/plugins"
)

target_sources(ultralightcursor PRIVATE
    lib/header/UltralightCursorEffect.hpp
    UltralightCursorEffect.cpp


    lib/UltralightHtmlEffect.cpp
    lib/header/UltralightHtmlEffect.hpp

    lib/KwinMouseProvider.cpp
    lib/header/KwinMouseProvider.hpp
    lib/Quick/PluginPath/PluginPath.hpp
    lib/Quick/PluginPath/PluginPath.cpp

    lib/UserConfig.cpp
    lib/header/UserConfig.hpp
    lib/header/MouseProvider.hpp
)




target_include_directories(ultralightcursor
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/header
)



target_link_libraries(ultralightcursor
    PRIVATE
    Qt6::Core
    Qt6::Gui
    KF6::CoreAddons
    KF6::ConfigCore
    KWin::kwin
    Ultralight::SDK
)


set_target_properties(ultralightcursor PROPERTIES PREFIX "")

install(
    FILES metadata.json
    DESTINATION "${CMAKE_INSTALL_DATADIR}/kwin/effects/ultralightcursor"
)
