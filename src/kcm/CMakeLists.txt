set(kcm_SRCS
    ShapeCornersKCM.h
    ShapeCornersKCM.cpp
    plugin.cpp
)

kconfig_add_kcfg_files(kcm_SRCS ../ShapeCornersConfig.kcfgc)

if(${QT_VERSION_MAJOR} EQUAL 6)
    qt6_wrap_ui(kcm_SRCS ShapeCornersKCM.ui)
    qt6_add_dbus_interface(kcm_SRCS ${KWIN_EFFECTS_INTERFACE} kwineffects_interface)
    kcoreaddons_add_plugin(kwin_shapecorners_config INSTALL_NAMESPACE "kwin/effects/configs" SOURCES ${kcm_SRCS})

    target_link_libraries(kwin_shapecorners_config
            Qt${QT_VERSION_MAJOR}::CorePrivate
            KF${QT_VERSION_MAJOR}::KCMUtils
    )
else()
    qt5_wrap_ui(kcm_SRCS ShapeCornersKCM.ui)
    qt5_add_dbus_interface(kcm_SRCS ${KWIN_EFFECTS_INTERFACE} kwineffects_interface)
    kcoreaddons_add_plugin(kwin_shapecorners_config INSTALL_NAMESPACE "kwin/effects/configs" SOURCES ${kcm_SRCS})
endif ()

target_link_libraries(kwin_shapecorners_config
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::DBus
    Qt${QT_VERSION_MAJOR}::Gui
    KF${QT_VERSION_MAJOR}::ConfigWidgets
    KF${QT_VERSION_MAJOR}::ConfigGui
    KF${QT_VERSION_MAJOR}::GlobalAccel
    KF${QT_VERSION_MAJOR}::WindowSystem
)