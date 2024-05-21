set(kcm_SRCS
        KCM.h
        KCM.cpp
        plugin.cpp
)

kconfig_add_kcfg_files(kcm_SRCS Config.kcfgc)

if(${QT_MAJOR_VERSION} EQUAL 6)
    qt6_wrap_ui(kcm_SRCS KCM.ui)
    qt6_add_dbus_interface(kcm_SRCS ${KWIN_EFFECTS_INTERFACE} kwineffects_interface)
    kcoreaddons_add_plugin(kwin_shapecorners_config INSTALL_NAMESPACE "kwin/effects/configs" SOURCES ${kcm_SRCS})

    target_link_libraries(kwin_shapecorners_config
            Qt${QT_MAJOR_VERSION}::CorePrivate
            KF${QT_MAJOR_VERSION}::KCMUtils
    )
else()
    qt5_wrap_ui(kcm_SRCS KCM.ui)
    qt5_add_dbus_interface(kcm_SRCS ${KWIN_EFFECTS_INTERFACE} kwineffects_interface)
    kcoreaddons_add_plugin(kwin_shapecorners_config INSTALL_NAMESPACE "kwin/effects/configs" SOURCES ${kcm_SRCS})
endif ()

target_link_libraries(kwin_shapecorners_config
    Qt${QT_MAJOR_VERSION}::DBus
    KF${QT_MAJOR_VERSION}::ConfigWidgets
)
