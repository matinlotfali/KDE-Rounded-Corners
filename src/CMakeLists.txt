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
    Qt6::Core
    Qt6::CorePrivate
    Qt6::Gui
    Qt6::DBus
    KWin::kwin
    epoxy::epoxy
    KF6::ConfigCore
    KF6::ConfigGui
    KF6::CoreAddons
    KF6::WindowSystem
)

install(TARGETS kwin4_effect_shapecorners DESTINATION ${PLUGIN_INSTALL_DIR}/kwin/effects/plugins/)

execute_process(COMMAND kf5-config --install data OUTPUT_VARIABLE DATAPATH OUTPUT_STRIP_TRAILING_WHITESPACE)
install(FILES shaders_140/shapecorners.frag DESTINATION ${DATAPATH}/kwin/shaders/1.40)
install(FILES shaders_110/shapecorners.frag DESTINATION ${DATAPATH}/kwin/shaders/1.10)