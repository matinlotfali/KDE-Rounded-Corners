set(effect_SRCS
    Effect.h
    Effect.cpp
    Shader.h
    Shader.cpp
    Window.h
    Window.cpp
    TileChecker.h
    TileChecker.cpp
    Color.h
    plugin.cpp
)

kconfig_add_kcfg_files(effect_SRCS Config.kcfgc)
kcoreaddons_add_plugin(kwin4_effect_shapecorners SOURCES ${effect_SRCS} INSTALL_NAMESPACE "kwin/effects/plugins/")

target_link_libraries(kwin4_effect_shapecorners
    Qt${QT_MAJOR_VERSION}::DBus
    KF${QT_MAJOR_VERSION}::ConfigGui
)
if(${QT_MAJOR_VERSION} EQUAL 6)
    target_link_libraries(kwin4_effect_shapecorners
            KWin::kwin
            Qt${QT_MAJOR_VERSION}::CorePrivate
    )
else()
    target_link_libraries(kwin4_effect_shapecorners
            KWinEffects::kwineffects
            KWinEffects::kwinglutils
    )
endif()

add_subdirectory(kcm)
add_subdirectory(shaders)