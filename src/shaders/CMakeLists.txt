set(SOURCE_SHADER "shapecorners_qt${QT_MAJOR_VERSION}.frag")
set(SOURCE_SHADER_CORE "shapecorners_qt${QT_MAJOR_VERSION}_core.frag")

set(SED_SHADOWS_COMMAND sed -i -e "/#include \"shapecorners_shadows.glsl\"/ { r ${CMAKE_SOURCE_DIR}/src/shaders/shapecorners_shadows.glsl" -e "d" -e "}")
set(SED_MERGE_GLSL_COMMAND sed -e "/#include \"shapecorners.glsl\"/ { r ${CMAKE_SOURCE_DIR}/src/shaders/shapecorners.glsl" -e "d" -e "}")

add_custom_target(
    shaders ALL
    COMMAND ${SED_MERGE_GLSL_COMMAND} "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_SHADER}" > "${CMAKE_CURRENT_BINARY_DIR}/shapecorners.frag"
    COMMAND ${SED_MERGE_GLSL_COMMAND} "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_SHADER_CORE}" > "${CMAKE_CURRENT_BINARY_DIR}/shapecorners_core.frag"
    COMMAND ${SED_SHADOWS_COMMAND} "${CMAKE_CURRENT_BINARY_DIR}/shapecorners.frag"
    COMMAND ${SED_SHADOWS_COMMAND} "${CMAKE_CURRENT_BINARY_DIR}/shapecorners_core.frag"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_SHADER}"
        "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_SHADER_CORE}"
    SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_SHADER}"
        "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_SHADER_CORE}"
    BYPRODUCTS
        "${CMAKE_CURRENT_BINARY_DIR}/shapecorners.frag"
        "${CMAKE_CURRENT_BINARY_DIR}/shapecorners_core.frag"
    VERBATIM
    COMMENT "Merging shapecorners.glsl: shapecorners.frag and shapecorners_core.frag"
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/shapecorners.frag"
        "${CMAKE_CURRENT_BINARY_DIR}/shapecorners_core.frag"
    DESTINATION "${KDE_INSTALL_DATADIR}/${KWIN_NAMESPACE_PREFIX}/shaders"
)
