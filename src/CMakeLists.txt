add_subdirectory(kcm)

set(effect_SRCS
    ShapeCornersEffect.h
    ShapeCornersEffect.cpp
    ShapeCornersShader.h
    ShapeCornersShader.cpp
    plugin.cpp
)

kconfig_add_kcfg_files(effect_SRCS ShapeCornersConfig.kcfgc)
add_library(kwin4_effect_shapecorners SHARED ${effect_SRCS})

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

install(TARGETS kwin4_effect_shapecorners DESTINATION ${PLUGIN_INSTALL_DIR}/kwin/effects/plugins/)

execute_process(COMMAND kf${QT_VERSION_MAJOR}-config --install data OUTPUT_VARIABLE DATAPATH OUTPUT_STRIP_TRAILING_WHITESPACE)
install(FILES shaders/shapecorners_core.frag DESTINATION ${DATAPATH}/kwin/shaders)
install(FILES shaders/shapecorners.frag DESTINATION ${DATAPATH}/kwin/shaders)