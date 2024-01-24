add_subdirectory(kcm)

set(effect_SRCS
    ShapeCornersEffect.h
    ShapeCornersEffect.cpp
    ShapeCornersShader.h
    ShapeCornersShader.cpp
    plugin.cpp
)

kconfig_add_kcfg_files(effect_SRCS ShapeCornersConfig.kcfgc)
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

install(DIRECTORY shaders DESTINATION ${KDE_INSTALL_DATADIR}/kwin)
