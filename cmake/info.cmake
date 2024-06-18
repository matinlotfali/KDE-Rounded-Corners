execute_process(
        COMMAND sh -c "ldconfig -v 2>&1 | awk '$1 ~ \"libkwin\" {print $1,$2,$3}'"
        OUTPUT_VARIABLE KWIN_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE
)
message("--- KWin Version: ${KWIN_VERSION}")

if(${QT_MAJOR_VERSION} EQUAL 6)
    set(KWIN_EFFECT_INCLUDE_FILE "/usr/include/kwin/effect/effect.h")
else ()
    set(KWIN_EFFECT_INCLUDE_FILE "/usr/include/kwineffects.h")
endif ()
execute_process(
        COMMAND sh -c "grep '#define KWIN_EFFECT_API_VERSION_MINOR' ${KWIN_EFFECT_INCLUDE_FILE} | awk '{print \$NF}'"
        OUTPUT_VARIABLE KWIN_EFFECT_API_VERSION_MINOR OUTPUT_STRIP_TRAILING_WHITESPACE
)
message("--- KWinEffect Version: ${KWIN_EFFECT_API_VERSION_MINOR}")

message("")