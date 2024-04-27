if(${QT_MAJOR_VERSION} EQUAL 6)
    set(KWIN_EFFECT_INCLUDE_FILE "/usr/include/kwin/effect/effect.h")
else ()
    set(KWIN_EFFECT_INCLUDE_FILE "/usr/include/kwineffects.h")
endif ()

execute_process(
        COMMAND sh -c "grep '#define KWIN_EFFECT_API_VERSION_MINOR' ${KWIN_EFFECT_INCLUDE_FILE} | awk '{print \$NF}'"
        OUTPUT_VARIABLE KWIN_EFFECT_API_VERSION_MINOR OUTPUT_STRIP_TRAILING_WHITESPACE
)
message("KWinEffect API Version: ${KWIN_EFFECT_API_VERSION_MINOR}")
message("")