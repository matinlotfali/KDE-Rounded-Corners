if ctest > /dev/null; then
    echo "KDE-Rounded-Corners is supported by KWin and doesn't need re-installation."
else
    kdialog --msgbox "KDE-Rounded-Corners is not supported by KWin anymore.\n\nThis can probably be for an update.\nWe will now rebuild and reinstall the effect."
    make clean
    cmake --build . -j
    kdialog --password "Enter password to install KDE-Rounded-Corners: " | sudo -S cmake --install .

    sh ../tools/load.sh
    if ctest > /dev/null; then
      kdialog --msgbox "KDE-Rounded-Corners is now installed and loaded."
    else
      kdialog --error "The installation was not successful.\nSee logs by compiling manually."
    fi
fi