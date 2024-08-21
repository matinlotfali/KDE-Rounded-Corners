cmake_minimum_required(VERSION 3.20)

project(kwin4_effect_shapecorners VERSION 0.6.2 LANGUAGES CXX)

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

# required frameworks by Core
find_package(KF${QT_MAJOR_VERSION} ${KF_MIN_VERSION} REQUIRED COMPONENTS ConfigWidgets)

if(${QT_MAJOR_VERSION} EQUAL 6)
    find_package(KF${QT_MAJOR_VERSION} ${KF_MIN_VERSION} REQUIRED COMPONENTS KCMUtils)
    find_package(KWin REQUIRED COMPONENTS kwineffects)
    message(STATUS "Found KWin Version: ${KWin_VERSION}")
else()
    find_package(KWinEffects REQUIRED COMPONENTS kwineffects kwinglutils)
    message(STATUS "Found KWin Version: ${KWinEffects_VERSION}")
endif ()
include(cmake/info.cmake)
find_package(epoxy REQUIRED)
find_package(XCB REQUIRED COMPONENTS XCB)
find_package(KWinDBusInterface CONFIG REQUIRED)

add_subdirectory(src)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

include(cmake/test.cmake)
include(cmake/pack.cmake)
