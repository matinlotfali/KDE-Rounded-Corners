#!/bin/bash

set -e

if [ -z "$KDE_SESSION_VERSION" ]; then
    echo "Not in a KDE Plasma environment to check the desktop effect \"KDE-Rounded-Corners\" support."
    exit 1;
fi

if ctest > /dev/null; then
    echo "Desktop effect \"KDE-Rounded-Corners\" is compatible with KWin and doesn't need re-installation."
else
    kdialog --msgbox "Desktop effect \"KDE-Rounded-Corners\" is not compatible with KWin anymore.\n\nThis can probably be for an update.\nWe will now rebuild and reinstall the effect."
    p=$(kdialog --progressbar "Building KDE-Rounded-Corners...")
    qdbus-qt6 $p showCancelButton false
    qdbus-qt6 $p Set "" maximum "26"

    rm -rf ./*
    qdbus-qt6 $p Set "" value "1"

    cmake .. --install-prefix /usr
    qdbus-qt6 $p Set "" value "2"

    cmake --build . -j &
    pid=$!
    # Sleep for a short duration to let progress files to be created
    sleep 0.5

    # Loop until the cmake --build process is complete
    while kill -0 $pid 2>/dev/null; do
        maximum=$(( $(tail -n 1 CMakeFiles/Progress/count.txt 2>/dev/null || :) + 4))
        if [ "$maximum" -gt "0" ]; then
          value=$(( $(find CMakeFiles/Progress/ | wc -l) + 2))
          qdbus-qt6 $p Set "" value "$value"
          qdbus-qt6 $p Set "" maximum "$maximum"
        fi

        # Sleep for a short duration to avoid excessive CPU usage
        sleep 0.1
    done
    qdbus-qt6 $p close


    kdialog --password "Enter password to install KDE-Rounded-Corners: " | sudo -S cmake --install .

    sh ../tools/load.sh
    if ctest > /dev/null; then
      kdialog --msgbox "Desktop effect \"KDE-Rounded-Corners\" is now installed and loaded."
    else
      kdialog --error "The installation was not successful.\nSee logs by compiling manually."
    fi
fi
