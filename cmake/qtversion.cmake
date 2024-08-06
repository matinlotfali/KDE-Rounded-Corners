execute_process(
        COMMAND sh -c "ldconfig -v 2>&1 | awk '$1 ~ \"libkwin.so\" {print $3}' | cut -d '.' -f 3-"
        OUTPUT_VARIABLE KWIN_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE
)
message("-- KWin Version: ${KWIN_VERSION}")

set(KF_MIN_VERSION "5.78")
set(QT_MIN_VERSION "5.15")
set(QT_MAJOR_VERSION 5)

if(KWIN_VERSION VERSION_GREATER "6")
    set(QT_MIN_VERSION "6.4.0")
    set(QT_MAJOR_VERSION 6)
    set(KF_MIN_VERSION 6)
endif()