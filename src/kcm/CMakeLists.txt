# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcmcorners\")

set(kcm_SRCS
        KCM.h
        KCM.cpp
        plugin.cpp
)

kconfig_add_kcfg_files(kcm_SRCS Config.kcfgc)

ki18n_wrap_ui(kcm_SRCS KCM.ui)
qt_add_dbus_interface(kcm_SRCS ${KWIN_EFFECTS_INTERFACE} kwineffects_interface)
kcoreaddons_add_plugin(kwin_shapecorners_config INSTALL_NAMESPACE "kwin/effects/configs" SOURCES ${kcm_SRCS})

if(${QT_MAJOR_VERSION} EQUAL 6)
    target_link_libraries(kwin_shapecorners_config
            Qt${QT_MAJOR_VERSION}::CorePrivate
            KF${QT_MAJOR_VERSION}::KCMUtils
    )
endif ()

target_link_libraries(kwin_shapecorners_config
    Qt${QT_MAJOR_VERSION}::DBus
    KF${QT_MAJOR_VERSION}::ConfigWidgets
    KF${QT_MAJOR_VERSION}::I18n
    KF${QT_MAJOR_VERSION}::I18nLocaleData
)

# Add compatibility for KWin 6.4.0 on X11
if(${KWinX11_FOUND})
    install(TARGETS kwin_shapecorners_config DESTINATION ${KDE_INSTALL_PLUGINDIR}/kwin-x11/effects/configs)
endif ()