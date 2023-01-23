# KDE-Rounded-Corners

This effect rounds the corners of your windows and adds an outline around them without much affecting the performance of the KDE Plasma desktop (see [#49](https://github.com/matinlotfali/KDE-Rounded-Corners/pull/49) and [#50](https://github.com/matinlotfali/KDE-Rounded-Corners/issues/50)).

This effect started as a fork of [shapecorners](https://sourceforge.net/projects/shapecorners/) with some additional contributions in [Alex47's project](https://github.com/alex47/KDE-Rounded-Corners), then I optimized and reimplemented the effect with shaders.

#### Tested on:
- ![Kubuntu 22.04 Jammy](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/kubuntu2204.yml?label=Kubuntu%2022.04%20Jammy&logo=kubuntu&branch=master) [![Deb Binary](https://img.shields.io/github/downloads/matinlotfali/KDE-Rounded-Corners/latest/kwin4-effect-shapecorners-kubuntu2204-amd64.deb?label=Download%20Binary)](https://github.com/matinlotfali/KDE-Rounded-Corners/releases/download/v0.2.0/kwin4-effect-shapecorners-kubuntu2204-amd64.deb)
- ![KDE Neon (Stable)](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/neon.yml?branch=master&label=KDE%20Neon%20%28Stable%29&logo=kde&logoColor=white)
- ![KDE Neon (Unstable)](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/neon-unstable.yml?branch=master&label=KDE%20Neon%20%28Unstable%29&logo=kde&logoColor=white)
- ![Debian 11 Bullseye](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/debian11.yml?branch=master&label=Debian%2011%20Bullseye&logo=debian) [![Deb Binary](https://img.shields.io/github/downloads/matinlotfali/KDE-Rounded-Corners/latest/kwin4-effect-shapecorners-debian11-amd64.deb?label=Download%20Binary)](https://github.com/matinlotfali/KDE-Rounded-Corners/releases/download/v0.2.0/kwin4-effect-shapecorners-debian11-amd64.deb)
- [![Arch](https://img.shields.io/badge/-manual-lightgrey?label=Arch%20Linux&logo=archlinux&logoColor=white) ![](https://img.shields.io/aur/maintainer/kde-rounded-corners?label=AUR%20Maintainer) ![](https://img.shields.io/aur/votes/kde-rounded-corners?label=AUR%20Votes)](https://aur.archlinux.org/packages/kde-rounded-corners)

![After](https://raw.githubusercontent.com/matinlotfali/KDE-Rounded-Corners/master/screenshots/outlines.png)

# Contributions:

- Compatibility of the effect with other effects like Wobbly windows - by [matinlotfali](https://github.com/matinlotfali)
- Optimize the effect to render once instead of 5 times - by [matinlotfali](https://github.com/matinlotfali) - see [#49](https://github.com/matinlotfali/KDE-Rounded-Corners/pull/49)
- Reimplementation with shaders to include shadows at corners and an outline - by [matinlotfali](https://github.com/matinlotfali)
- Compatiblity with KWin from versions 5.23 to 5.26 - by [matinlotfali](https://github.com/matinlotfali)
- Disable effect when window gets maximized - by [matinlotfali](https://github.com/matinlotfali)
- Cleanups for the plugin logic, remove unneeded dependencies from CMakeLists.txt file - by [alex1701c](https://github.com/alex1701c)
- Separate outline color for active and inactive windows - by [OrkenWhite](https://github.com/OrkenWhite)

# How to build from source code:

You need to install development packages for your distrobution first:

**Debian based (Ubuntu, Kubuntu, KDE Neon)** - by [alex47](https://github.com/alex47):
```
sudo apt install git cmake g++ gettext extra-cmake-modules qttools5-dev libqt5x11extras5-dev libkf5configwidgets-dev libkf5globalaccel-dev libkf5notifications-dev kwin-dev 
```
**Fedora** - by [matinlotfali](https://github.com/matinlotfali)
```
sudo dnf install git cmake gcc-c++ extra-cmake-modules qt5-qttools-devel qt5-qttools-static qt5-qtx11extras-devel kf5-kconfigwidgets-devel kf5-kcrash-devel kf5-kguiaddons-devel kf5-kglobalaccel-devel kf5-kio-devel kf5-ki18n-devel kwin-devel qt5-qtbase-devel libepoxy-devel
```
**Arch** - by [hexa-one](https://github.com/hexa-one)
```
sudo pacman -S git cmake extra-cmake-modules base-devel
yay -S qt5-tools
```
or AUR package by [Shaurya-Kalia](https://github.com/Shaurya-Kalia)

```
sudo pamac build kde-rounded-corners
```
**OpenSUSE** - by [mathiasgredal](https://github.com/mathiasgredal)
```
sudo zypper install git cmake gcc-c++ extra-cmake-modules libqt5-qttools-devel libqt5-qtx11extras-devel kconfigwidgets-devel kguiaddons-devel kglobalaccel-devel ki18n-devel knotifications-devel kwin5-devel libQt5Gui-devel libQt5OpenGL-devel libepoxy-devel
```
**Void** - by [lay-by](https://github.com/lay-by)
```
xbps-install git cmake make qt5-tools-devel extra-cmake-modules qt5-x11extras-devel gettext-devel kwin-devel
```
Then get the source code and compile:
```
git clone https://github.com/matinlotfali/KDE-Rounded-Corners
cd KDE-Rounded-Corners
mkdir qt5build
cd qt5build
cmake ../ -DCMAKE_INSTALL_PREFIX=/usr -DQT5BUILD=ON
make
sudo make install
```

You can now logout and log back in or run the command below to have it activated.
```
kwin_x11 --replace &
```

# Settings

You can change corner radius, or disable the effect in:

> System Settings --> Workspace Behavior --> Desktop Effects --> ShapeCorners

To fully uninstall, simply run the command `sudo make uninstall` inside `qt5build` directory
