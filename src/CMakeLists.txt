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
    Qt5::Core
    Qt5::Gui
    Qt5::DBus
    KWinEffects::kwineffects
    KWinEffects::kwinglutils
    epoxy::epoxy
    KF5::ConfigCore
    KF5::ConfigGui
    KF5::CoreAddons
    KF5::WindowSystem
)

install(TARGETS kwin4_effect_shapecorners DESTINATION ${PLUGIN_INSTALL_DIR}/kwin/effects/plugins/)

execute_process(COMMAND kf5-config --install data OUTPUT_VARIABLE DATAPATH OUTPUT_STRIP_TRAILING_WHITESPACE)
install(FILES shaders_140/shapecorners.frag DESTINATION ${DATAPATH}/kwin/shaders/1.40)
install(FILES shaders_110/shapecorners.frag DESTINATION ${DATAPATH}/kwin/shaders/1.10)