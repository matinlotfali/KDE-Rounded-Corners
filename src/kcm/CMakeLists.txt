set(kcm_SRCS
    ShapeCornersKCM.cpp
    plugin.cpp
)

kconfig_add_kcfg_files(kcm_SRCS ../ShapeCornersConfig.kcfgc)

qt5_wrap_ui(kcm_SRCS ShapeCornersKCM.ui)

qt5_add_dbus_interface(kcm_SRCS ${KWIN_EFFECTS_INTERFACE} kwineffects_interface)

add_library(kwin_shapecorners_config MODULE ${kcm_SRCS})

target_link_libraries(kwin_shapecorners_config
    Qt5::Core
    Qt5::DBus
    Qt5::Gui
    KF5::ConfigWidgets
    KF5::ConfigGui
    KF5::GlobalAccel
    KF5::WindowSystem
)

install(TARGETS kwin_shapecorners_config DESTINATION ${PLUGIN_INSTALL_DIR}/kwin/effects/configs)