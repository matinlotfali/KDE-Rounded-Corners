find_package(KF6 QUIET COMPONENTS ConfigWidgets)

if(${KF6_FOUND} EQUAL 0)
    set(QT_MIN_VERSION "5.15")
    set(QT_MAJOR_VERSION 5)
    set(KF_MIN_VERSION "5.78")
endif ()
