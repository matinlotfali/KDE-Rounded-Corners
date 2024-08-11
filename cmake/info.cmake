if(${KF_MIN_VERSION} EQUAL 6)
    set(KWIN_EFFECT_INCLUDE_FILE "/usr/include/kwin/effect/effect.h")
else ()
    set(KWIN_EFFECT_INCLUDE_FILE "/usr/include/kwineffects.h")
endif ()
execute_process(
        COMMAND sh -c "grep '#define KWIN_EFFECT_API_VERSION_MINOR' ${KWIN_EFFECT_INCLUDE_FILE} | awk '{print \$NF}'"
        OUTPUT_VARIABLE KWIN_EFFECT_API_VERSION_MINOR OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "Found KWinEffect API Version: ${KWIN_EFFECT_API_VERSION_MINOR}")

#below is a very useful way of finding variables and contains:

#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    string(TOLOWER "${_variableName}" KEY)
#    string(TOLOWER "${${_variableName}}" VALUE)
#    string(FIND "${KEY}" "kwin" INDEX1)
#    string(FIND "${VALUE}" "kwin" INDEX2)
#    if (${INDEX1} GREATER -1 OR ${INDEX2} GREATER -1)
#        message(STATUS "VARIABLE ${_variableName}=${${_variableName}}")
#    endif ()
#endforeach()
