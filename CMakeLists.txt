cmake_minimum_required(VERSION 3.20)

project(kwin4_effect_shapecorners VERSION 0.6.2 LANGUAGES CXX)

include(cmake/default-vars.cmake)
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
else()
    find_package(KWinEffects REQUIRED COMPONENTS kwineffects kwinglutils)
endif ()
find_package(epoxy REQUIRED)
find_package(XCB REQUIRED COMPONENTS XCB)
find_package(KWinDBusInterface CONFIG REQUIRED)

add_subdirectory(src)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

include(cmake/info.cmake)
include(cmake/test.cmake)
include(cmake/pack.cmake)
