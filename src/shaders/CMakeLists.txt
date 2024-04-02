if(${QT_MAJOR_VERSION} EQUAL 6)
    install(FILES shapecorners_qt6.frag DESTINATION ${KDE_INSTALL_DATADIR}/kwin/shaders RENAME shapecorners.frag)
    install(FILES shapecorners_qt6_core.frag DESTINATION ${KDE_INSTALL_DATADIR}/kwin/shaders RENAME shapecorners_core.frag)
else()
    install(FILES shapecorners_qt5.frag DESTINATION ${KDE_INSTALL_DATADIR}/kwin/shaders RENAME shapecorners.frag)
    install(FILES shapecorners_qt5_core.frag DESTINATION ${KDE_INSTALL_DATADIR}/kwin/shaders RENAME shapecorners_core.frag)
endif()

install(CODE "
    set(SED_COMMAND sed -e \"/#include \\\"shapecorners.glsl\\\"/ {\" -e \"r ${CMAKE_SOURCE_DIR}/src/shaders/shapecorners.glsl\" -e \"d\" -e \"}\" -i)
    message(STATUS \"Merging shapecorners.glsl: shapecorners.frag\")
    execute_process(
        COMMAND \${SED_COMMAND} shapecorners.frag
        WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${KDE_INSTALL_DATADIR}/kwin/shaders
    )
    message(STATUS \"Merging shapecorners.glsl: shapecorners_core.frag\")
    execute_process(
        COMMAND \${SED_COMMAND} shapecorners_core.frag
        WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${KDE_INSTALL_DATADIR}/kwin/shaders
    )
")