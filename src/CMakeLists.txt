add_subdirectory(kcm)

set(effect_SRCS
    ShapeCornersEffect.h
    ShapeCornersEffect.cpp
    ShapeCornersShader.h
    ShapeCornersShader.cpp
    plugin.cpp
)

kconfig_add_kcfg_files(effect_SRCS ShapeCornersConfig.kcfgc)
kcoreaddons_add_plugin(kwin4_effect_shapecorners INSTALL_NAMESPACE "kwin/effects/plugins/")
target_sources(kwin4_effect_shapecorners PRIVATE ${effect_SRCS})

target_link_libraries(kwin4_effect_shapecorners
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::DBus
    epoxy::epoxy
    KF${QT_VERSION_MAJOR}::ConfigCore
    KF${QT_VERSION_MAJOR}::ConfigGui
    KF${QT_VERSION_MAJOR}::CoreAddons
    KF${QT_VERSION_MAJOR}::WindowSystem
)
if(${QT_VERSION_MAJOR} EQUAL 6)
    target_link_libraries(kwin4_effect_shapecorners
            KWin::kwin
            Qt${QT_VERSION_MAJOR}::CorePrivate
    )
else()
    target_link_libraries(kwin4_effect_shapecorners
            KWinEffects::kwineffects
            KWinEffects::kwinglutils
    )
endif()

install(DIRECTORY shaders DESTINATION ${KDE_INSTALL_DATADIR}/kwin)
