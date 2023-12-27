cmake_minimum_required(VERSION 3.20)

project(${SHAPECORNERS})

set(KF_MIN_VERSION "5.240.0")
set(QT_MIN_VERSION "6.4.0")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DQT_NO_DEBUG_OUTPUT")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${ECM_MODULE_PATH}
    ${ECM_KDE_MODULE_DIR}
)

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS
        Core
        Gui
        DBus
        Widgets
        OpenGL
        Network
        Xml
)

# required frameworks by Core
find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    Config
    ConfigWidgets
    CoreAddons
    GlobalAccel
    WindowSystem
    KCMUtils
)

find_package(epoxy REQUIRED)
find_package(XCB REQUIRED COMPONENTS XCB)

find_package(KWin REQUIRED COMPONENTS
        kwineffects
)

find_package(KWinDBusInterface CONFIG REQUIRED)

add_subdirectory(src)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

# these are cache variables, so they could be overwritten with -D,
set(CPACK_PACKAGE_NAME kwin4-effect-shapecorners
    CACHE STRING kwin4-effect-shapecorners
)
set(CPACK_DEBIAN_FILE_NAME "kwin4-effect-shapecorners-kubuntu2204-amd64.deb")
set(CPACK_PACKAGE_CONTACT "matinlotfali@gmail.com")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Matin Lotfaliei")
set(CPACK_DEBIAN_PACKAGE_VERSION "0.3.0")

# autogenerate dependency information
set (CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)

# To generate deb files, install 'dpkg-dev' package and then run 'cpack -G DEB'
