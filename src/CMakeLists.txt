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
    install(FILES shaders/shapecorners_qt6.frag DESTINATION ${KDE_INSTALL_DATADIR}/kwin/shaders RENAME shaders/shapecorners.frag)
    install(FILES shaders/shapecorners_qt6_core.frag DESTINATION ${KDE_INSTALL_DATADIR}/kwin/shaders RENAME shaders/shapecorners_core.frag)
else()
    target_link_libraries(kwin4_effect_shapecorners
            KWinEffects::kwineffects
            KWinEffects::kwinglutils
    )
    install(FILES shaders/shapecorners_qt5.frag DESTINATION ${KDE_INSTALL_DATADIR}/kwin/shaders RENAME shaders/shapecorners.frag)
    install(FILES shaders/shapecorners_qt5_core.frag DESTINATION ${KDE_INSTALL_DATADIR}/kwin/shaders RENAME shaders/shapecorners_core.frag)
endif()
install(FILES shaders/shapecorners.glsl DESTINATION ${KDE_INSTALL_DATADIR}/kwin/shaders)
