# KDE-Rounded-Corners

This effect rounds the corners of your windows and adds an outline around them without much affecting the performance of the KDE Plasma desktop (see [#49](https://github.com/matinlotfali/KDE-Rounded-Corners/pull/49) and [#50](https://github.com/matinlotfali/KDE-Rounded-Corners/issues/50)).

This effect started as a fork of [shapecorners](https://sourceforge.net/projects/shapecorners/) with some additional contributions in [Alex47's project](https://github.com/alex47/KDE-Rounded-Corners), then I optimized and reimplemented the effect with shaders with influences from the [invert effect](https://github.com/KDE/kwin/tree/master/src/plugins/invert).

**Tested on:**
- ![Kubuntu 22.04 Jammy](https://img.shields.io/badge/-not_supported-red?label=Kubuntu%2022.04%20Jammy&logo=kubuntu&branch=master) 
![](https://img.shields.io/badge/KWinEffects-v233-lightgrey)
- ![Kubuntu 22.04 Jammy + Backports](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/kubuntu2204-backports.yml?label=Kubuntu%2022.04%20Jammy%20+%20Backports&logo=kubuntu&branch=master) 
![](https://img.shields.io/badge/KWinEffects-v234-lightgrey)
- ![Debian 12 Bookworm](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/debian12.yml?branch=master&label=Debian%2012%20Bookworm&logo=debian)
    ![](https://img.shields.io/badge/KWinEffects-v236-lightgrey)
- ![KDE Neon (Stable)](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/neon.yml?branch=master&label=KDE%20Neon%20%28Stable%29&logo=kde&logoColor=white)
![](https://img.shields.io/badge/KWinEffects-v236-lightgrey)
- ![KDE Neon (Unstable)](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/neon-unstable.yml?branch=master&label=KDE%20Neon%20%28Unstable%29&logo=kde&logoColor=white)
![](https://img.shields.io/badge/KWinEffects-v237-lightgrey)
- [![Arch](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/arch.yml?branch=master&label=Arch%20Linux&logo=archlinux&logoColor=white) ![](https://img.shields.io/aur/maintainer/kwin-effect-rounded-corners-git?label=AUR%20Maintainer) ![](https://img.shields.io/aur/votes/kwin-effect-rounded-corners-git?label=AUR%20Votes)](https://aur.archlinux.org/packages/kwin-effect-rounded-corners-git)

![After](screenshots/shadows2.png)

# Contributions:

- Compatibility of the effect with other effects like Wobbly windows - by [matinlotfali](https://github.com/matinlotfali)
- Optimize the effect to render once instead of 5 times - by [matinlotfali](https://github.com/matinlotfali) - see [#49](https://github.com/matinlotfali/KDE-Rounded-Corners/pull/49)
- Reimplementation with shaders to include shadows at corners and an outline - by [matinlotfali](https://github.com/matinlotfali)
- Compatibility with KWin from versions 5.23 to 5.27 - by [matinlotfali](https://github.com/matinlotfali)
- Disable effect when window gets maximized - by [matinlotfali](https://github.com/matinlotfali)
- Cleanups for the plugin logic, remove unneeded dependencies from CMakeLists.txt file - by [alex1701c](https://github.com/alex1701c)
- Separate outline color for active and inactive windows - by [OrkenWhite](https://github.com/OrkenWhite)
- Add natural shadows using parametric blend - by [paletteOvO](https://github.com/paletteOvO)

# How to build from source code:

You need to install development packages for your distribution first:

- **Debian based (Ubuntu, Kubuntu, KDE Neon)** - by [alex47](https://github.com/alex47):
  ```
  sudo apt install git cmake g++ gettext extra-cmake-modules qttools5-dev libqt5x11extras5-dev libkf5configwidgets-dev libkf5globalaccel-dev libkf5notifications-dev kwin-dev 
  ```
- **Fedora** - by [matinlotfali](https://github.com/matinlotfali)
  ```
  sudo dnf install git cmake gcc-c++ extra-cmake-modules qt5-qttools-devel qt5-qttools-static qt5-qtx11extras-devel kf5-kconfigwidgets-devel kf5-kcrash-devel kf5-kguiaddons-devel kf5-kglobalaccel-devel kf5-kio-devel kf5-ki18n-devel kwin-devel qt5-qtbase-devel libepoxy-devel
  ```
- **Arch** - by [hexa-one](https://github.com/hexa-one)
  ```
  sudo pacman -S git cmake extra-cmake-modules base-devel
  yay -S qt5-tools
  ```
  or AUR package by [xiota](https://aur.archlinux.org/account/xiota)  
  ```
  sudo pamac build kwin-effect-rounded-corners-git
  ```
- **OpenSUSE** - by [mathiasgredal](https://github.com/mathiasgredal) and [Richardsause](https://github.com/Richardsause)
  ```
  sudo zypper install git cmake gcc-c++ extra-cmake-modules libqt5-qttools-devel libqt5-qtx11extras-devel kconfigwidgets-devel kguiaddons-devel kglobalaccel-devel ki18n-devel knotifications-devel kwin5-devel libQt5Gui-devel libQt5OpenGL-devel libepoxy-devel kwindowsystem-devel libqt5-qtnetworkauth-devel
  ```
- **Void** - by [lay-by](https://github.com/lay-by)
  ```
  xbps-install git cmake make qt5-tools-devel extra-cmake-modules qt5-x11extras-devel gettext-devel kwin-devel
  ```
- **NixOS** - by [Pavel Zolotarevskiy](https://github.com/flexagoon)
   ```
   nix-env -iA nixos.kde-rounded-corners
   ```
    

Then get the source code and compile:
```
git clone https://github.com/matinlotfali/KDE-Rounded-Corners
cd KDE-Rounded-Corners
mkdir build
cd build
cmake .. --install-prefix /usr
make
sudo make install
```

You can now logout and log back in or run the command below to have it activated.
```
kwin --replace &
```

> [!CAUTION]
> Running the command above in **Wayland** restarts your session and closes all your current open windows.

# Settings

You can change the corner radius, or disable the effect in:

> [ System Settings ] --> [ Workspace Behavior ] --> [ Desktop Effects ] --> [ ShapeCorners ]

To fully uninstall, simply run the command `sudo make uninstall` inside the `build` directory

# Tips

## Add shadow to windows without decoration (like Steam)

You can add shadows for specific windows using the hack below. I don't know how to enforce it in my code.

1. In [ System settings ] -> [ Window management ] -> [ Window rules ] -> [ Appearance & Fixes ]:

   **Add [steam] and set [ No titlebar ] and frame to [ No ]**
   
3. In [ System settings ] -> [ Application Style ] -> [ Window decoration ] -> [ Breeze theme setting ] -> [ Window specific overrides ]:

   **Add [steam] and set [ Hide Window title bar ] to [ Yes ].**

After that, the Steam window gets its shadows back.
