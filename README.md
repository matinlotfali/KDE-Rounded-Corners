# KDE-Rounded-Corners

This effect rounds the corners of your windows. Window borders should be turned off for the effect to work. [How?](#settings)

This is a fork of [shapecorners](https://sourceforge.net/projects/shapecorners/) with some additional contributions in [Alex47's project](https://github.com/alex47/KDE-Rounded-Corners).

[![Donate](https://img.shields.io/badge/Wish%20me%20be%20more%20active%20on%20this%3F%20Motivate%20me%20with-Paypal-blue)](https://www.paypal.com/donate/?business=WYGJ6T4J23P2L&no_recurring=0&currency_code=CAD)

#### Tested on:
- ![Kubuntu 22.04 Jammy](https://img.shields.io/github/workflow/status/matinlotfali/KDE-Rounded-Corners/Kubuntu%2022.04?label=Kubuntu%2022.04%20Jammy&logo=kubuntu)
- ![Kubuntu 20.04 Focal](https://img.shields.io/github/workflow/status/matinlotfali/KDE-Rounded-Corners/Kubuntu%2020.04?label=Kubuntu%2020.04%20Focal&logo=kubuntu) [![Release Binary](https://img.shields.io/github/downloads/matinlotfali/KDE-Rounded-Corners/latest/kwin4-effect-shapecorners-kubuntu2004-amd64.deb?label=release%20binary)](https://github.com/matinlotfali/KDE-Rounded-Corners/releases/download/v0.1.1/kwin4-effect-shapecorners-kubuntu2004-amd64.deb)
- ![KDE Neon (Stable)](https://img.shields.io/github/workflow/status/matinlotfali/KDE-Rounded-Corners/KDE%20Neon%20(Stable)?label=KDE%20Neon%20%28Stable%29&logo=kde&logoColor=white)
- ![KDE Neon (Unstable)](https://img.shields.io/github/workflow/status/matinlotfali/KDE-Rounded-Corners/KDE%20Neon%20(Unstable)?label=KDE%20Neon%20%28Unstable%29&logo=kde&logoColor=white)
- ![Debian 11 Bullseye](https://img.shields.io/github/workflow/status/matinlotfali/KDE-Rounded-Corners/Debian?label=Debian%2011%20Bullseye&logo=debian) [![Release Binary](https://img.shields.io/github/downloads/matinlotfali/KDE-Rounded-Corners/latest/kwin4-effect-shapecorners-debian11-amd64.deb?label=release%20binary)](https://github.com/matinlotfali/KDE-Rounded-Corners/releases/download/v0.1.1/kwin4-effect-shapecorners-kubuntu2004-amd64.deb)
- [![Arch](https://img.shields.io/github/workflow/status/matinlotfali/KDE-Rounded-Corners/Debian?label=Arch%20Linux&logo=archlinux&logoColor=white) ![](https://img.shields.io/aur/maintainer/kde-rounded-corners?label=AUR%20Maintainer) ![](https://img.shields.io/aur/votes/kde-rounded-corners?label=AUR%20Votes)](https://aur.archlinux.org/packages/kde-rounded-corners)

![After](https://raw.githubusercontent.com/alex47/KDE-Rounded-Corners/master/screenshots/after.PNG)

# Contributions:

- Compatible with KWin 5.23 and 5.24 - by [matinlotfali](https://github.com/matinlotfali)
- Disable effect when window gets maximized - by [matinlotfali](https://github.com/matinlotfali)
- Added Latte Dock to ignore list - by [gikari](https://github.com/gikari)

# How to build from source code:

You need to install development packages for your distrobution first:

**Debian based (Ubuntu, Kubuntu, KDE Neon)** - by [alex47](https://github.com/alex47):
```
sudo apt install git cmake g++ gettext extra-cmake-modules qttools5-dev libqt5x11extras5-dev libkf5configwidgets-dev libkf5crash-dev libkf5globalaccel-dev libkf5kio-dev libkf5notifications-dev kinit-dev kwin-dev 
```
**Fedora** - by [matinlotfali](https://github.com/matinlotfali)
```
sudo dnf install git cmake gcc-c++ extra-cmake-modules qt5-qttools-devel qt5-qttools-static qt5-qtx11extras-devel kf5-kconfigwidgets-devel kf5-kcrash-devel kf5-kguiaddons-devel kf5-kglobalaccel-devel kf5-kio-devel kf5-ki18n-devel kf5-knotifications-devel kf5-kinit-devel kwin-devel qt5-qtbase-devel libepoxy-devel
```
**Arch** - by [hexa-one](https://github.com/hexa-one)
```
sudo pacman -S git cmake extra-cmake-modules base-devel kinit
yay -S qt5-tools
```
or AUR package by [Shaurya-Kalia](https://github.com/Shaurya-Kalia)

```
sudo pamac build kde-rounded-corners
```
**OpenSUSE** - by [mathiasgredal](https://github.com/mathiasgredal)
```
sudo zypper install git cmake gcc-c++ extra-cmake-modules libqt5-qttools-devel libqt5-qtx11extras-devel kconfigwidgets-devel kcrash-devel kguiaddons-devel kglobalaccel-devel kio-devel ki18n-devel knotifications-devel kinit-devel kwin5-devel libQt5Gui-devel libQt5OpenGL-devel libepoxy-devel
```
**Void** - by [lay-by](https://github.com/lay-by)
```
xbps-install git cmake make kinit qt5-tools-devel extra-cmake-modules qt5-x11extras-devel kinit-devel gettext-devel kwin-devel
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

You need to turn off the border size in:

> System Settings --> Application Style --> Window Decoration

You can change corner radius, or disable the effect in:

> System Settings --> Workspace Behavior --> Desktop Effects --> ShapeCorners

To fully uninstall, simply run the command `sudo make uninstall` inside `qt5build` directory
