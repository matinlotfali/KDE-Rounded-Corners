# KDE-Rounded-Corners

This is a fork of https://sourceforge.net/projects/shapecorners/ with additional contributions in https://github.com/alex47/KDE-Rounded-Corners

When you disable window borders in System Settings --> Application Style --> Window Decorations --> Border size: No Borders you get sharp bottom window corners.

This effect rounds the corners of your windows.


![After](https://raw.githubusercontent.com/alex47/KDE-Rounded-Corners/master/screenshots/after.PNG)

# Contributions:

- Disable effect when window gets maximized - by [ciampolo](https://github.com/ciampolo)
- Added Latte Dock to ignore list - by [gikari](https://github.com/gikari)

# Dependencies:
Debian based (Ubuntu, Kubuntu, KDE Neon) - by [alex47](https://github.com/alex47):
```
sudo apt install git cmake g++ gettext extra-cmake-modules qttools5-dev libqt5x11extras5-dev libkf5configwidgets-dev libkf5crash-dev libkf5globalaccel-dev libkf5kio-dev libkf5notifications-dev kinit-dev kwin-dev 
```
Fedora based - by [matinlotfali](https://github.com/matinlotfali)
```
sudo dnf install git cmake gcc-c++ extra-cmake-modules qt5-qttools-devel qt5-qttools-static qt5-qtx11extras-devel kf5-kconfigwidgets-devel kf5-kcrash-devel kf5-kguiaddons-devel kf5-kglobalaccel-devel kf5-kio-devel kf5-ki18n-devel kf5-knotifications-devel kf5-kinit-devel kwin-devel qt5-qtbase-devel libepoxy-devel
```

Arch based - by [hexa-one](https://github.com/hexa-one)
```
sudo pacman -S git cmake extra-cmake-modules base-devel kinit
yay -S qt5-tools
```

OpenSUSE based - by [mathiasgredal](https://github.com/mathiasgredal)
```
sudo zypper install git cmake gcc-c++ extra-cmake-modules libqt5-qttools-devel libqt5-qtx11extras-devel kconfigwidgets-devel kcrash-devel kguiaddons-devel kglobalaccel-devel kio-devel ki18n-devel knotifications-devel kinit-devel kwin5-devel libQt5Gui-devel libQt5OpenGL-devel libepoxy-devel
```

# How to build:
```
git clone https://github.com/matinlotfali/KDE-Rounded-Corners

cd KDE-Rounded-Corners; mkdir qt5build; cd qt5build; cmake ../ -DCMAKE_INSTALL_PREFIX=/usr -DQT5BUILD=ON && make && sudo make install && (kwin_x11 --replace &)
```


It should be now activated.

For better results turn off the border size in:

System Settings --> Application Style --> Window Decoration

# Settings

You can change corner radius, or disable the effect in:

System Settings --> Workspace Behavior --> Desktop Effects --> ShapeCorners

To fully uninstall, simply run the command `sudo make uninstall` inside `qt5build` directory
