#!/bin/bash

if [ -z "$KDE_SESSION_VERSION" ]; then
    echo "Not in a KDE Plasma environment to check the KDE-Rounded-Corners support"
    exit 1;
fi

if ctest > /dev/null; then
    echo "KDE-Rounded-Corners is supported by KWin and doesn't need re-installation."
else
    kdialog --msgbox "KDE-Rounded-Corners is not supported by KWin anymore.\n\nThis can probably be for an update.\nWe will now rebuild and reinstall the effect."
    rm -rf ./*
    cmake .. --install-prefix /usr
    cmake --build . -j &
    pid=$!

    p=$(kdialog --progressbar "Building KDE-Rounded-Corners...")
    qdbus $p showCancelButton false

    # Loop until the cmake --build process is complete
    while kill -0 $pid 2>/dev/null; do
        count=$(( $(tail -n 1 CMakeFiles/Progress/count.txt 2>/dev/null) + 2))
        if [ "$count" -gt "0" ]; then
          value=$(find CMakeFiles/Progress/ | wc -l)
          qdbus $p Set "" value "$value"
          qdbus $p Set "" maximum "$count"
        fi

        # Sleep for a short duration to avoid excessive CPU usage
        sleep 0.1
    done
    qdbus $p close


    kdialog --password "Enter password to install KDE-Rounded-Corners: " | sudo -S cmake --install .

    sh ../tools/load.sh
    if ctest > /dev/null; then
      kdialog --msgbox "KDE-Rounded-Corners is now installed and loaded."
    else
      kdialog --error "The installation was not successful.\nSee logs by compiling manually."
    fi
fi
