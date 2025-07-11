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

option(DEBUG_ANIMATION "Add debug logs for animation" OFF)
option(DEBUG_INCLUSIONS "Add debug logs for inclusion & exclusion lists" OFF)
option(DEBUG_MAXIMIZED "Add debug logs for maximized windows" OFF)

if (DEBUG_ANIMATION OR DEBUG_INCLUSIONS OR DEBUG_MAXIMIZED)
    set(CMAKE_BUILD_TYPE "Debug")
    target_compile_definitions(kwin4_effect_shapecorners PRIVATE QT_DEBUG)
endif ()

if (DEBUG_ANIMATION)
    message("-- Debug: Animation debug logs are enabled.")
    target_compile_definitions(kwin4_effect_shapecorners PRIVATE DEBUG_ANIMATION)
endif (DEBUG_ANIMATION)

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
