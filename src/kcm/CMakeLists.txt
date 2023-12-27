set(kcm_SRCS
    ShapeCornersKCM.h
    ShapeCornersKCM.cpp
    plugin.cpp
)

kconfig_add_kcfg_files(kcm_SRCS ../ShapeCornersConfig.kcfgc)

qt6_wrap_ui(kcm_SRCS ShapeCornersKCM.ui)

qt6_add_dbus_interface(kcm_SRCS ${KWIN_EFFECTS_INTERFACE} kwineffects_interface)

add_library(kwin_shapecorners_config MODULE ${kcm_SRCS})

target_link_libraries(kwin_shapecorners_config
    Qt6::Core
    Qt6::CorePrivate
    Qt6::DBus
    Qt6::Gui
    KF6::ConfigWidgets
    KF6::ConfigGui
    KF6::GlobalAccel
    KF6::WindowSystem
    KF6::KCMUtils
)

install(TARGETS kwin_shapecorners_config DESTINATION ${PLUGIN_INSTALL_DIR}/kwin/effects/configs)