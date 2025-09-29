cmake_minimum_required(VERSION 3.20)

project(kwin4_effect_shapecorners VERSION 0.7.2 LANGUAGES CXX)

include(cmake/default-vars.cmake)

find_package(ECM "5.100" REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
        ${ECM_MODULE_PATH}
        ${ECM_KDE_MODULE_DIR}
)
include(cmake/qtversion.cmake)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(cmake/qtversion.cmake)

# required frameworks by Core
find_package(KF${QT_MAJOR_VERSION} ${KF_MIN_VERSION} REQUIRED COMPONENTS ConfigWidgets I18n)

if(${QT_MAJOR_VERSION} EQUAL 6)
    find_package(Qt6 REQUIRED COMPONENTS DBus)

    if(${Qt6_VERSION} VERSION_GREATER_EQUAL 6.10)
        find_package(Qt6 REQUIRED COMPONENTS CorePrivate)
    endif()

    find_package(KF${QT_MAJOR_VERSION} ${KF_MIN_VERSION} REQUIRED COMPONENTS KCMUtils)

    option(KWIN_X11 "Build for KWin X11 instead of Wayland" OFF)
    if (KWIN_X11)
        find_package(KWinX11 REQUIRED COMPONENTS kwineffects)
        message(STATUS "Found KWinX11 Version: ${KWinX11_VERSION}")
        set (CMAKE_PACKAGE_NAME "${CMAKE_PROJECT_NAME}_x11")
        set (CMAKE_PROJECT_VERSION "${CMAKE_PROJECT_VERSION}.${KWinX11_VERSION}")
        set (KWIN_LIB_NAME KWinX11)
        set (KWIN_NAMESPACE_PREFIX "kwin-x11")
    else ()
        find_package(KWin REQUIRED COMPONENTS kwineffects)
        message(STATUS "Found KWin Version: ${KWin_VERSION}")
        set (CMAKE_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
        set (CMAKE_PROJECT_VERSION "${CMAKE_PROJECT_VERSION}.${KWin_VERSION}")
        set (KWIN_LIB_NAME KWin)
        set (KWIN_NAMESPACE_PREFIX "kwin")
    endif ()
else()
    find_package(KWinEffects REQUIRED COMPONENTS kwineffects kwinglutils)
    message(STATUS "Found KWin Version: ${KWinEffects_VERSION}")
    set (CMAKE_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
    set (CMAKE_PROJECT_VERSION "${CMAKE_PROJECT_VERSION}.${KWinEffects_VERSION}")
    set (KWIN_NAMESPACE_PREFIX "kwin")
endif ()
include(cmake/info.cmake)
find_package(epoxy REQUIRED)
find_package(XCB REQUIRED COMPONENTS XCB)
find_package(KWinDBusInterface CONFIG REQUIRED)

add_subdirectory(src)
ki18n_install(po)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

include(cmake/test.cmake)
include(cmake/pack.cmake)
