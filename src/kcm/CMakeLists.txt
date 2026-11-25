set(kcm_SRCS
        KCM.h
        KCM.cpp
        plugin.cpp
)

kconfig_add_kcfg_files(kcm_SRCS Config.kcfgc)

ki18n_wrap_ui(kcm_SRCS KCM.ui)
qt_add_dbus_interface(kcm_SRCS ${KWIN_EFFECTS_INTERFACE} kwineffects_interface)
kcoreaddons_add_plugin(kwin_shapecorners_config INSTALL_NAMESPACE "${KWIN_NAMESPACE_PREFIX}/effects/configs" SOURCES ${kcm_SRCS})

# KI18N Translation Domain for this library
target_compile_definitions(kwin_shapecorners_config PRIVATE TRANSLATION_DOMAIN=\"kcmcorners\")

target_link_libraries(kwin_shapecorners_config
    Qt${QT_MAJOR_VERSION}::CorePrivate
    KF${QT_MAJOR_VERSION}::KCMUtils
    Qt${QT_MAJOR_VERSION}::DBus
    KF${QT_MAJOR_VERSION}::ConfigWidgets
    KF${QT_MAJOR_VERSION}::I18n
    KF${QT_MAJOR_VERSION}::I18nLocaleData
)
