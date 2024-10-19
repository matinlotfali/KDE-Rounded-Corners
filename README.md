# KDE-Rounded-Corners

This effect rounds the corners of your windows and adds an outline around them without much affecting the performance of the KDE Plasma desktop (see [#49](https://github.com/matinlotfali/KDE-Rounded-Corners/pull/49) and [#50](https://github.com/matinlotfali/KDE-Rounded-Corners/issues/50)).

This effect started as a fork of [shapecorners](https://sourceforge.net/projects/shapecorners/) with some additional contributions in [Alex47's project](https://github.com/alex47/KDE-Rounded-Corners), then I optimized and reimplemented the effect with shaders with influences from the [invert effect](https://github.com/KDE/kwin/tree/master/src/plugins/invert).

**Tested on:**
* ![Wayland](https://img.shields.io/badge/Wayland-supported-green?logo=wayland) ![Wayland](https://img.shields.io/badge/X11-supported-green?logo=X.org)
- ![Kubuntu 22.04 Jammy](https://img.shields.io/badge/-not_supported-red?label=Kubuntu%2022.04&logo=kubuntu&branch=master)
      &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
      ![](https://img.shields.io/badge/Plasma-5.24-red)
- ![Kubuntu 22.04 Jammy + Backports](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/kubuntu2204-backports.yml?label=Kubuntu%2022.04%20%2b%20Backports%20PPA&logo=kubuntu&branch=master)
      ![](https://img.shields.io/badge/Plasma-5.27-lightgrey)
      [![](https://img.shields.io/sourceforge/dm/kde-rounded-corners/nightly%2Fkubuntu2204?label=Download%20%5Bkwin4_effect_shapecorners_kubuntu2204.deb%5D)](https://sourceforge.net/projects/kde-rounded-corners/files/nightly/kubuntu2204/kwin4_effect_shapecorners_kubuntu2204.deb/download)
- ![Kubuntu 24.04 Noble](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/kubuntu2404.yml?label=Kubuntu%2024.04&logo=kubuntu&branch=master)
      &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
      ![](https://img.shields.io/badge/Plasma-5.27-lightgrey)
      [![](https://img.shields.io/sourceforge/dm/kde-rounded-corners/nightly%2Fkubuntu?label=Download%20%5Bkwin4_effect_shapecorners_kubuntu2404.deb%5D)](https://sourceforge.net/projects/kde-rounded-corners/files/nightly/kubuntu/kwin4_effect_shapecorners_kubuntu2404.deb/download)
- ![Kubuntu 24.10 Oracular](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/kubuntu2410.yml?label=Kubuntu%2024.10&logo=kubuntu&branch=master)
    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
    ![](https://img.shields.io/badge/Plasma-6.1-lightgreen)
    [![](https://img.shields.io/sourceforge/dm/kde-rounded-corners/nightly%2Fkubuntu2410?label=Download%20%5Bkwin4_effect_shapecorners_kubuntu2410.deb%5D)](https://sourceforge.net/projects/kde-rounded-corners/files/nightly/kubuntu2410/kwin4_effect_shapecorners_kubuntu2410.deb/download)
- ![Debian 12 Bookworm](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/debian12.yml?branch=master&label=Debian%2012&logo=debian)
      &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
      &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
      ![](https://img.shields.io/badge/Plasma-5.27-lightgrey)
      [![](https://img.shields.io/sourceforge/dm/kde-rounded-corners/nightly%2Fdebian?label=Download%20%5Bkwin4_effect_shapecorners_debian12.deb%5D)](https://sourceforge.net/projects/kde-rounded-corners/files/nightly/debian/kwin4_effect_shapecorners_debian12.deb/download)
- ![Debian Sid](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/debian-sid.yml?branch=master&label=Debian%20Sid%20(Unstable)&logo=debian)
    [![#269](https://img.shields.io/badge/%23269-blue)](https://github.com/matinlotfali/KDE-Rounded-Corners/pull/269)
    ![](https://img.shields.io/badge/Plasma-5.27-lightgrey)
- ![openSUSE Tumbleweed](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/fedora40.yml?branch=master&label=openSUSE%20Tumbleweed&logo=opensuse&logoColor=white)
      &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
      ![](https://img.shields.io/badge/Plasma-6.2-green)
      [![](https://img.shields.io/sourceforge/dm/kde-rounded-corners/nightly%2Ftumbleweed?label=Download%20%5Bkwin4_effect_shapecorners_tumbleweed.rpm%5D)](https://sourceforge.net/projects/kde-rounded-corners/files/nightly/tumbleweed/kwin4_effect_shapecorners_tumbleweed.rpm/download)
- ![Fedora 40](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/fedora40.yml?branch=master&label=Fedora%2040&logo=fedora&logoColor=white)
      &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
      &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
      ![](https://img.shields.io/badge/Plasma-6.2-green)
      [![](https://img.shields.io/sourceforge/dm/kde-rounded-corners/nightly%2Ffedora?label=Download%20%5Bkwin4_effect_shapecorners_fedora40.rpm%5D)](https://sourceforge.net/projects/kde-rounded-corners/files/nightly/fedora/kwin4_effect_shapecorners_fedora40.rpm/download)
- ![Arch](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/arch.yml?branch=master&label=Arch%20Linux&logo=archlinux&logoColor=white)
      &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
      &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
      ![](https://img.shields.io/badge/Plasma-6.2-green)
      [![](https://img.shields.io/aur/maintainer/kwin-effect-rounded-corners-git?label=AUR%20Maintainer)](https://aur.archlinux.org/packages/kwin-effect-rounded-corners-git)
      [![](https://img.shields.io/aur/votes/kwin-effect-rounded-corners-git?label=AUR%20Votes)](https://aur.archlinux.org/packages/kwin-effect-rounded-corners-git)
      [![](https://img.shields.io/aur/popularity/kwin-effect-rounded-corners-git?label=AUR%20Popularity)](https://aur.archlinux.org/packages/kwin-effect-rounded-corners-git)
- ![KDE Neon (Stable)](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/neon.yml?branch=master&label=KDE%20Neon%20%28Stable%29&logo=kde&logoColor=white)
      &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
      ![](https://img.shields.io/badge/Plasma-6.2-green)
      [![](https://img.shields.io/sourceforge/dm/kde-rounded-corners/nightly%2Fneon?label=Download%20%5Bkwin4_effect_shapecorners_neon.deb%5D)](https://sourceforge.net/projects/kde-rounded-corners/files/nightly/neon/kwin4_effect_shapecorners_neon.deb/download)
- ![KDE Neon (Unstable)](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/neon-unstable.yml?branch=master&label=KDE%20Neon%20%28Unstable%29&logo=kde&logoColor=white)
      &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
      ![](https://img.shields.io/badge/Plasma-6.2-green)

https://github.com/matinlotfali/KDE-Rounded-Corners/assets/7337168/7081f1c8-9fa5-440f-90b5-28b2fb04546a

# Contributions:

- Compatibility of the effect with other effects like Wobbly windows
- Compatibility with KWin for Plasma versions 5.27 to 6.0
- Compatibility with HDR in Plasma 6.0
- Optimize the effect to render once instead of 5 times - see [#49](https://github.com/matinlotfali/KDE-Rounded-Corners/pull/49)
- Smooth animation when a window moves to an active state
- Reimplementation with shaders to include shadows at corners and an outline
- Ability to disable effect when windows get maximized or tiled
- Cleanups for the plugin logic, remove unneeded dependencies from CMakeLists.txt file - by [alex1701c](https://github.com/alex1701c)
- Separate outline color for active and inactive windows - by [OrkenWhite](https://github.com/OrkenWhite)
- Add natural shadows using parametric blend - by [paletteOvO](https://github.com/paletteOvO)

# How to build from source code:

You need to install development packages for your distribution first:

<details>
<summary>Debian based (Ubuntu, Kubuntu, KDE Neon)</summary>
<br>
    
  - Plasma 5 - by [alex47](https://github.com/alex47):
    ```
    sudo apt install git cmake g++ extra-cmake-modules kwin-dev libkf5configwidgets-dev 
    ```
  - Plasma 6
    ```
    sudo apt install git cmake g++ extra-cmake-modules kwin-dev qt6-base-private-dev qt6-base-dev-tools libkf6kcmutils-dev
    ```
</details>
<details>
<summary>Fedora</summary>
<br>

 - Plasma 5 (Fedora 39)
   ```bash
   sudo dnf install git cmake gcc-c++ extra-cmake-modules kwin-devel kf5-kconfigwidgets-devel libepoxy-devel
   ```
 - Plasma 6 (Fedora 40 and later)
   ```bash
   sudo dnf install git cmake gcc-c++ extra-cmake-modules kwin-devel kf6-kconfigwidgets-devel libepoxy-devel kf6-kcmutils-devel qt6-qtbase-private-devel wayland-devel
   ```
</details>
<details>
<summary>Arch - by https://github.com/hexa-one</summary>

  ```
  sudo pacman -S git cmake extra-cmake-modules base-devel
  yay -S qt5-tools
  ```
  or AUR package by [xiota](https://aur.archlinux.org/account/xiota)  
  ```
  sudo pamac build kwin-effect-rounded-corners-git
  ```
</details>
<details>
<summary>OpenSUSE - by https://github.com/mathiasgredal, https://github.com/Richardsause, and https://github.com/aaronkirschen</summary>
<br>

 - Plasma 5 (by https://github.com/mathiasgredal, https://github.com/Richardsause)
  ```
  sudo zypper install git cmake gcc-c++ extra-cmake-modules libqt5-qttools-devel kconfigwidgets-devel kwindowsystem-devel kguiaddons-devel ki18n-devel knotifications-devel kwin5-devel libQt5Gui-devel libQt5OpenGL-devel libepoxy-devel libqt5-qtnetworkauth-devel
  ```
 - Plasma 6 (by https://github.com/aaronkirschen)
  ```
  sudo zypper in git cmake gcc-c++ kf6-kconfigwidgets-devel kf6-kcmutils-devel kwin6-devel kf6-kwindowsystem-devel qt6-quick-devel qt6-core-private-devel
  ```
</details>
<details>
<summary>Void - by https://github.com/lay-by</summary>

  ```
  xbps-install git cmake make qt5-tools-devel extra-cmake-modules gettext-devel kwin-devel
  ```
</details>
<details>
<summary>NixOS - by https://github.com/flexagoon</summary>

   ```
   nix-env -iA nixos.kde-rounded-corners
   ```
</details>

Then clone the source code and compile it:
```bash
git clone https://github.com/matinlotfali/KDE-Rounded-Corners
cd KDE-Rounded-Corners
mkdir build
cd build
cmake ..
cmake --build . -j
sudo make install
```

# Load & Unload

To activate the effect, you can now log out and log back in, or run the command below inside the `build` directory:
```bash
sh ../tools/load.sh
```

To fully uninstall the effect, run the following commands inside the `build` directory:

```bash
sh ../tools/unload.sh
sudo make uninstall
```

# Auto install after KWin update

After each `kwin` package update, the effect becomes incompatible. So it won't load without a rebuild.

As long as the effect is not part of the `kwin` yet (being discussed 
[here](https://invent.kde.org/plasma/kwin/-/issues/198)), you can automate the re-installation by running the command
below inside the `build` directory:

```bash
sh ../tools/install-autorun-test.sh
```

The command above adds a `desktop` file inside the `autorun` directory which checks if the effect is still supported,
if it is not supported, it will automatically rebuild and reinstall the effect.

> [!NOTE]
> The script uses `qdbus` to show a progress bar. On Plasma 6, it is not installed by default. You need to manually install the package `qtchooser`.

# Settings

You can change the corner radius, or disable the effect in:

> [ System Settings ] --> [ Workspace Behavior ] --> [ Desktop Effects ] --> [ ShapeCorners ]

# Tips

## Disable conflicting native window outline

If using Breeze (default) window decorations with Plasma 5.27 or higher you may wish to disable the native window outline, to prevent it from overlapping and causing visual glitches.

- System settings -> Themes -> Window Decorations -> Breeze -> Edit icon -> Shadows and Outline tab -> Outline intensity (Off)

## Add shadow to windows without decoration (like Steam)

You can add shadows for specific windows using the hack below. I don't know how to enforce it in my code.

1. In [ System settings ] -> [ Window management ] -> [ Window rules ] -> [ Appearance & Fixes ]:

   **Add [steam] and set [ No titlebar ] and frame to [ No ]**
   
2. In [ System settings ] -> [ Application Style ] -> [ Window decoration ] -> [ Breeze theme setting ] -> [ Window specific overrides ]:

   **Add [steam] and set [ Hide Window title bar ] to [ Yes ].**

After that, the Steam window gets its shadows back.

## Add Debug Messages

When troubleshooting or reporting an issue, it might be useful to enable Debug logs during the build time using:

```bash
cmake .. --DCMAKE_BUILD_TYPE=Debug
cmake --build . -j
```

After the installation and loading the effect, debug messages would appear in `journalctl`:

```bash
journalctl -f | grep kwin
```

or have some colorful logs with

```bash
sh ../tools/show-kwin-logs.sh
```
