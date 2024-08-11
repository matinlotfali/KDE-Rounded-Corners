find_package(KF6 QUIET COMPONENTS ConfigWidgets)

if(KF6_FOUND)
    set(QT_MIN_VERSION "6.4.0")
    set(QT_MAJOR_VERSION 6)
    set(KF_MIN_VERSION 6)
endif (KF6_FOUND)
