set(SOURCE_SHADER "shapecorners_qt${QT_MAJOR_VERSION}.frag")
set(SOURCE_SHADER_CORE "shapecorners_qt${QT_MAJOR_VERSION}_core.frag")

set(SED_COMMAND sed -e "/#include \"shapecorners.glsl\"/ { r ${CMAKE_SOURCE_DIR}/src/shaders/shapecorners.glsl" -e "d" -e "}")

add_custom_target(
    shaders ALL
    DEPENDS shapecorners.frag shapecorners_core.frag
)
add_custom_command(
    OUTPUT
        shapecorners.frag
        shapecorners_core.frag
    COMMAND ${SED_COMMAND} "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_SHADER}" > "${CMAKE_CURRENT_BINARY_DIR}/shapecorners.frag"
    COMMAND ${SED_COMMAND} "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_SHADER_CORE}" > "${CMAKE_CURRENT_BINARY_DIR}/shapecorners_core.frag"
    DEPENDS
        "${SOURCE_SHADER}"
        "${SOURCE_SHADER_CORE}"
    VERBATIM
    COMMENT "Merging shapecorners.glsl: shapecorners.frag and shapecorners_core.frag"
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/shapecorners.frag"
        "${CMAKE_CURRENT_BINARY_DIR}/shapecorners_core.frag"
    DESTINATION "${KDE_INSTALL_DATADIR}/kwin/shaders"
)
