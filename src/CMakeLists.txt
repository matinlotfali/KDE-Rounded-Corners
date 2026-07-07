set(effect_SRCS
    Effect.h
    Effect.cpp
    Shader.h
    Shader.cpp
    Window.h
    Window.cpp
    TileChecker.h
    TileChecker.cpp
    FloatColor.h
    WindowConfig.h
    WindowConfig.cpp
    WindowManager.h
    WindowManager.cpp
    Animation.h
    Animation.cpp
    plugin.cpp
)

kconfig_add_kcfg_files(effect_SRCS Config.kcfgc)
kcoreaddons_add_plugin(kwin4_effect_shapecorners SOURCES ${effect_SRCS} INSTALL_NAMESPACE "${KWIN_NAMESPACE_PREFIX}/effects/plugins/")

target_link_libraries(kwin4_effect_shapecorners
    Qt${QT_MAJOR_VERSION}::DBus
    KF${QT_MAJOR_VERSION}::ConfigGui
)
if(${QT_MAJOR_VERSION} EQUAL 6)
    target_link_libraries(kwin4_effect_shapecorners
            ${KWIN_LIB_NAME}::kwin
            Qt${QT_MAJOR_VERSION}::CorePrivate
    )
else()
    target_link_libraries(kwin4_effect_shapecorners
            KWinEffects::kwineffects
            KWinEffects::kwinglutils
    )
endif()

string(REPLACE "." ";" _kwin_ver_list "${KWIN_VERSION}")
list(GET _kwin_ver_list 0 _kwin_ver_major)
list(GET _kwin_ver_list 1 _kwin_ver_minor)
list(GET _kwin_ver_list 2 _kwin_ver_patch)
math(EXPR _kwin_ver_num "(${_kwin_ver_major} << 16) | (${_kwin_ver_minor} << 8) | ${_kwin_ver_patch}")
target_compile_definitions(kwin4_effect_shapecorners PRIVATE KWIN_PLUGIN_VERSION_NUM=${_kwin_ver_num})

option(DEBUG_INCLUSIONS "Add debug logs for inclusion & exclusion lists" OFF)
option(DEBUG_MAXIMIZED "Add debug logs for maximized windows" OFF)

if (CMAKE_BUILD_TYPE EQUAL "Debug")
    message("-- Debug: Using debug build type.")
endif ()

if (DEBUG_INCLUSIONS OR DEBUG_MAXIMIZED)
    message("-- Debug: Switched to debug mode because of other flags.")
    set(CMAKE_BUILD_TYPE "Debug")
    target_compile_definitions(kwin4_effect_shapecorners PRIVATE QT_DEBUG)
endif ()

if (DEBUG_INCLUSIONS)
    message("-- Debug: Inclusion & exclusion debug logs are enabled.")
    target_compile_definitions(kwin4_effect_shapecorners PRIVATE DEBUG_INCLUSIONS)
endif (DEBUG_INCLUSIONS)

if (DEBUG_MAXIMIZED)
    message("-- Debug: Maximized debug logs are enabled.")
    target_compile_definitions(kwin4_effect_shapecorners PRIVATE DEBUG_MAXIMIZED)
endif (DEBUG_MAXIMIZED)

add_subdirectory(kcm)
add_subdirectory(shaders)
