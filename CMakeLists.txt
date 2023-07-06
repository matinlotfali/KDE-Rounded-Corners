cmake_minimum_required(VERSION 3.16)

project(kwin4-effect-shapecorners VERSION "0.4")

set(KF5_MIN_VERSION "5.78")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DQT_NO_DEBUG_OUTPUT")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${ECM_MODULE_PATH}
    ${ECM_KDE_MODULE_DIR}
)

find_package(Qt5 CONFIG REQUIRED COMPONENTS
    Gui
    Core
    DBus
    UiTools
    Widgets
    X11Extras
    OpenGL
    Network
    Xml
)

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

# required frameworks by Core
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    Config
    ConfigWidgets
    CoreAddons
    GlobalAccel
    WindowSystem
)

find_package(XCB REQUIRED COMPONENTS XCB)

find_package(KWinEffects REQUIRED COMPONENTS
    kwineffects
    kwinglutils
)

find_package(KWinDBusInterface CONFIG REQUIRED)
find_package(epoxy REQUIRED)
find_package(Git REQUIRED)
execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process (
    COMMAND bash -c "awk -F= '/^ID=/{print $2}' /etc/os-release |tr -d '\n' | tr -d '\"'"
    OUTPUT_VARIABLE OS_ID
)

add_subdirectory(src)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

# these are cache variables, so they could be overwritten with -D,
set(CPACK_PACKAGE_NAME ${CMAKE_PROJECT_NAME}
    CACHE STRING ${CMAKE_PROJECT_NAME}
)
set(CPACK_DEBIAN_PACKAGE_VERSION "${CMAKE_PROJECT_VERSION}.${GIT_HASH}")
set(CPACK_DEBIAN_FILE_NAME "${CMAKE_PROJECT_NAME}_${CPACK_DEBIAN_PACKAGE_VERSION}_${OS_ID}.deb")
set(CPACK_PACKAGE_CONTACT "matinlotfali@gmail.com")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Matin Lotfaliei")

# autogenerate dependency information
set (CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)

# To generate deb files, install 'dpkg-dev' package and then run 'cpack -G DEB'
