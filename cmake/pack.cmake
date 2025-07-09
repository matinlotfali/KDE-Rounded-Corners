# these are cache variables, so they could be overwritten with -D,
set(CPACK_PACKAGE_NAME ${CMAKE_PACKAGE_NAME} CACHE STRING ${CMAKE_PACKAGE_NAME})
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PACKAGE_NAME}")
set(CPACK_PACKAGE_VERSION "${CMAKE_PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Rounds the corners of your windows in KDE Plasma 5 and 6")
set(CPACK_PACKAGE_CONTACT "matinlotfali@gmail.com")

set(CPACK_RPM_PACKAGE_NAME "${CMAKE_PACKAGE_NAME}")
set(CPACK_RPM_PACKAGE_LICENSE "GPL-3.0")

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Matin Lotfaliei")
set(CPACK_DEBIAN_PACKAGE_SECTION "kde")

# autogenerate dependency information
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON)
set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS_POLICY "=")

include(CPack)
# To generate deb files, install 'dpkg-dev' package and then run 'cpack -G DEB'
