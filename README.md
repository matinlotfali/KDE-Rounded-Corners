# KDE-Rounded-Corners

This effect rounds the corners of your windows and adds an outline around them without much affecting the performance of the KDE Plasma desktop (see [#49](https://github.com/matinlotfali/KDE-Rounded-Corners/pull/49) and [#50](https://github.com/matinlotfali/KDE-Rounded-Corners/issues/50)).

This effect started as a fork of [shapecorners](https://sourceforge.net/projects/shapecorners/) with some additional contributions in [Alex47's project](https://github.com/alex47/KDE-Rounded-Corners), then I optimized and reimplemented the effect with shaders with influences from the [invert effect](https://github.com/KDE/kwin/tree/master/src/plugins/invert).

I maintain this effect on KDE Plasma desktop version 5.27 to 6.4+ in various Linux distros ([See tests](#tested-on)).
<br>[!["Buy Me A Coffee"](https://www.buymeacoffee.com/assets/img/custom_images/yellow_img.png)](https://www.buymeacoffee.com/matinlotfali)

![before-after](https://github.com/user-attachments/assets/cbd98412-ee47-4f4b-8b80-297328dfb1f5)

### Tested on
* ![Wayland](https://img.shields.io/badge/Wayland-supported-green?logo=wayland) ![Wayland](https://img.shields.io/badge/X11-supported-green?logo=X.org)
- ![Kubuntu 22.04 Jammy](https://img.shields.io/badge/-not_supported-red?label=Kubuntu%2022.04&logo=kubuntu&branch=master)
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  ![](https://img.shields.io/badge/Plasma-5.24-red)
- [![Kubuntu 22.04 Jammy + Backports](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/kubuntu2204-backports.yml?label=Kubuntu%2022.04%20%2b%20Backports%20PPA&logo=kubuntu&branch=master)](https://github.com/matinlotfali/KDE-Rounded-Corners/actions/workflows/kubuntu2204-backports.yml)
  ![](https://img.shields.io/badge/Plasma-5.27-lightgrey)
  [![](https://img.shields.io/sourceforge/dm/kde-rounded-corners/nightly%2Fkubuntu2204?label=Download%20%5Bkwin4_effect_shapecorners_kubuntu2204.deb%5D)](https://sourceforge.net/projects/kde-rounded-corners/files/nightly/kubuntu2204/kwin4_effect_shapecorners_kubuntu2204.deb/download)
- [![Kubuntu 24.04 Noble](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/kubuntu2404.yml?label=Kubuntu%2024.04&logo=kubuntu&branch=master)](https://github.com/matinlotfali/KDE-Rounded-Corners/actions/workflows/kubuntu2404.yml)
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  ![](https://img.shields.io/badge/Plasma-5.27-lightgrey)
  [![](https://img.shields.io/sourceforge/dm/kde-rounded-corners/nightly%2Fkubuntu?label=Download%20%5Bkwin4_effect_shapecorners_kubuntu2404.deb%5D)](https://sourceforge.net/projects/kde-rounded-corners/files/nightly/kubuntu/kwin4_effect_shapecorners_kubuntu2404.deb/download)
- [![Kubuntu 24.10 Oracular](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/kubuntu2410.yml?label=Kubuntu%2024.10&logo=kubuntu&branch=master)](https://github.com/matinlotfali/KDE-Rounded-Corners/actions/workflows/kubuntu2410.yml)
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  ![](https://img.shields.io/badge/Plasma-6.1-lightgreen)
  [![](https://img.shields.io/sourceforge/dm/kde-rounded-corners/nightly%2Fkubuntu2410?label=Download%20%5Bkwin4_effect_shapecorners_kubuntu2410.deb%5D)](https://sourceforge.net/projects/kde-rounded-corners/files/nightly/kubuntu2410/kwin4_effect_shapecorners_kubuntu2410.deb/download)
- [![Kubuntu 25.04 Plucky](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/kubuntu2504.yml?label=Kubuntu%2025.04&logo=kubuntu&branch=master)](https://github.com/matinlotfali/KDE-Rounded-Corners/actions/workflows/kubuntu2504.yml)
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  ![](https://img.shields.io/badge/Plasma-6.3-lightblue)
  [![](https://img.shields.io/sourceforge/dm/kde-rounded-corners/nightly%2Fkubuntu2504?label=Download%20%5Bkwin4_effect_shapecorners_kubuntu2504.deb%5D)](https://sourceforge.net/projects/kde-rounded-corners/files/nightly/kubuntu2504/kwin4_effect_shapecorners_kubuntu2504.deb/download)
- [![Debian 12 Bookworm](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/debian12.yml?branch=master&label=Debian%2012&logo=debian)](https://github.com/matinlotfali/KDE-Rounded-Corners/actions/workflows/debian12.yml)
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  ![](https://img.shields.io/badge/Plasma-5.27-lightgrey)
  [![](https://img.shields.io/sourceforge/dm/kde-rounded-corners/nightly%2Fdebian?label=Download%20%5Bkwin4_effect_shapecorners_debian12.deb%5D)](https://sourceforge.net/projects/kde-rounded-corners/files/nightly/debian/kwin4_effect_shapecorners_debian12.deb/download)
- [![Debian 13 Trixie](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/debian13.yml?branch=master&label=Debian%2013&logo=debian)](https://github.com/matinlotfali/KDE-Rounded-Corners/actions/workflows/debian13.yml)
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  ![](https://img.shields.io/badge/Plasma-6.3-lightblue)
  [![](https://img.shields.io/sourceforge/dm/kde-rounded-corners/nightly%2Fdebian13?label=Download%20%5Bkwin4_effect_shapecorners_debian13.deb%5D)](https://sourceforge.net/projects/kde-rounded-corners/files/nightly/debian13/kwin4_effect_shapecorners_debian13.deb/download)
- [![Debian Sid](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/debian-sid.yml?branch=master&label=Debian%20Sid%20(Unstable)&logo=debian)](https://github.com/matinlotfali/KDE-Rounded-Corners/actions/workflows/debian-sid.yml)
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  ![](https://img.shields.io/badge/Plasma-6.3-lightblue)
- [![openSUSE Tumbleweed](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/tumbleweed.yml?branch=master&label=openSUSE%20Tumbleweed&logo=opensuse&logoColor=white)](https://github.com/matinlotfali/KDE-Rounded-Corners/actions/workflows/tumbleweed.yml)
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  ![](https://img.shields.io/badge/Plasma-6.4-blue)
  ![](https://img.shields.io/badge/Copr-gray)[![Copr build status](https://copr.fedorainfracloud.org/coprs/matinlotfali/KDE-Rounded-Corners/package/kwin-effect-roundcorners/status_image/last_build.png)](https://copr.fedorainfracloud.org/coprs/matinlotfali/KDE-Rounded-Corners/package/kwin-effect-roundcorners/)
  [![](https://img.shields.io/sourceforge/dm/kde-rounded-corners/nightly%2Ftumbleweed?label=Download%20RPM)](https://sourceforge.net/projects/kde-rounded-corners/files/nightly/tumbleweed/kwin4_effect_shapecorners_tumbleweed.rpm/download)
- [![Fedora 42](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/fedora42.yml?branch=master&label=Fedora%2042&logo=fedora&logoColor=white)](https://github.com/matinlotfali/KDE-Rounded-Corners/actions/workflows/fedora42.yml)
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  ![](https://img.shields.io/badge/Plasma-6.4-blue)
  ![](https://img.shields.io/badge/Copr-gray)[![Copr build status](https://copr.fedorainfracloud.org/coprs/matinlotfali/KDE-Rounded-Corners/package/kwin-effect-roundcorners/status_image/last_build.png)](https://copr.fedorainfracloud.org/coprs/matinlotfali/KDE-Rounded-Corners/package/kwin-effect-roundcorners/)
  [![](https://img.shields.io/sourceforge/dm/kde-rounded-corners/nightly%2Ffedora42?label=Download%20RPM)](https://sourceforge.net/projects/kde-rounded-corners/files/nightly/fedora42/kwin4_effect_shapecorners_fedora42.rpm/download)
- [![Arch](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/arch.yml?branch=master&label=Arch%20Linux&logo=archlinux&logoColor=white)](https://github.com/matinlotfali/KDE-Rounded-Corners/actions/workflows/arch.yml)
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  ![](https://img.shields.io/badge/Plasma-6.4-blue)
  [![](https://img.shields.io/aur/maintainer/kwin-effect-rounded-corners-git?label=AUR%20Maintainer)](https://aur.archlinux.org/packages/kwin-effect-rounded-corners-git)
  [![](https://img.shields.io/aur/votes/kwin-effect-rounded-corners-git?label=AUR%20Votes)](https://aur.archlinux.org/packages/kwin-effect-rounded-corners-git)
  [![](https://img.shields.io/aur/popularity/kwin-effect-rounded-corners-git?label=AUR%20Popularity)](https://aur.archlinux.org/packages/kwin-effect-rounded-corners-git)
- [![KDE Neon (Stable)](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/neon.yml?branch=master&label=KDE%20Neon%20%28Stable%29&logo=kde&logoColor=white)](https://github.com/matinlotfali/KDE-Rounded-Corners/actions/workflows/neon.yml)
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  ![](https://img.shields.io/badge/Plasma-6.4-blue)
  [![](https://img.shields.io/sourceforge/dm/kde-rounded-corners/nightly%2Fneon?label=Download%20%5Bkwin4_effect_shapecorners_neon.deb%5D)](https://sourceforge.net/projects/kde-rounded-corners/files/nightly/neon/kwin4_effect_shapecorners_neon.deb/download)
- [![KDE Neon (Unstable)](https://img.shields.io/github/actions/workflow/status/matinlotfali/KDE-Rounded-Corners/neon-unstable.yml?branch=master&label=KDE%20Neon%20%28Unstable%29&logo=kde&logoColor=white)](https://github.com/matinlotfali/KDE-Rounded-Corners/actions/workflows/neon-unstable.yml)
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  ![](https://img.shields.io/badge/Plasma-6.4-blue)

# Contributions:

- Compatibility of the effect with other effects like Wobbly windows
- Compatibility with KWin for Plasma versions 5.27 to 6.4
- Compatibility with HDR in Plasma 6.0
- Compatibility with split KWin codebase dependency starting version 6.4 - see [#383](https://github.com/matinlotfali/KDE-Rounded-Corners/pull/383)
- Optimize the effect to render once instead of 5 times - see [#49](https://github.com/matinlotfali/KDE-Rounded-Corners/pull/49)
- Smooth animation when a window moves to an active state
- Reimplementation with shaders, including shadows at corners and two outlines
- Ability to disable the effect when windows get maximized or tiled
- Cleanups for the plugin logic, remove unneeded dependencies from CMakeLists.txt file - by [alex1701c](https://github.com/alex1701c)
- Separate outline color for active and inactive windows - by [OrkenWhite](https://github.com/OrkenWhite)
- Support for language translations - by [VictorR2007](https://github.com/VictorR2007) (See [How to add more translations?](#how-to-add-more-languages-to-the-translation))

<a href="https://github.com/matinlotfali/KDE-Rounded-Corners/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=matinlotfali/KDE-Rounded-Corners" />
</a>

# How to install using unofficial repositories

Copr package at [matinlotfali/KDE-Rounded-Corners](https://copr.fedorainfracloud.org/coprs/matinlotfali/KDE-Rounded-Corners)

```bash
sudo dnf copr enable matinlotfali/KDE-Rounded-Corners
sudo dnf install kwin-effect-roundcorners # or kwin-effect-roundcorners-x11 
```

AUR package by [xiota](https://aur.archlinux.org/account/xiota)

```bash
sudo pamac build kwin-effect-rounded-corners-git # or kwin-effect-rounded-corners-x11-git
```

NixOS package by [flexagoon](https://github.com/flexagoon)

```bash
nix-env -iA nixos.kde-rounded-corners
```


# How to install using a pre-built package

Download `.deb` or `.rpm` packages from above and install them using the commands:

```bash
sudo dpkg -i ./kwin4_effect_shapecorners_debian.deb # for debian and kubuntu
sudo dnf install ./kwin4_effect_shapecorners_fedora42.rpm # for fedora
sudo zypper install ./kwin4_effect_shapecorners_tumbleweed.rpm # for opensuse tumbleweed
```

# How to build from source code

You need to install development packages for your distribution first:

<details>
<summary>Debian based (Ubuntu, Kubuntu, KDE Neon)</summary>

- Plasma 6
  ```bash
  sudo apt install git cmake g++ extra-cmake-modules kwin-dev qt6-base-private-dev qt6-base-dev-tools libkf6kcmutils-dev
  ```
- Plasma 5 - by [alex47](https://github.com/alex47):
  ```bash
  sudo apt install git cmake g++ extra-cmake-modules kwin-dev libkf5configwidgets-dev 
  ```

</details>
<details>
<summary>Fedora</summary>

- Plasma 6 (Fedora 40 and later)
   ```bash
   sudo dnf install git cmake gcc-c++ extra-cmake-modules kwin-devel kf6-kconfigwidgets-devel libepoxy-devel kf6-kcmutils-devel kf6-ki18n-devel qt6-qtbase-private-devel wayland-devel libdrm-devel
   ``` 
- Plasma 5 (Fedora 39)
   ```bash
   sudo dnf install git cmake gcc-c++ extra-cmake-modules kwin-devel kf5-kconfigwidgets-devel kf5-ki18n-devel libepoxy-devel
   ```

</details>
<details>
<summary>Arch - by https://github.com/hexa-one</summary>

  ```bash
  sudo pacman -S git cmake extra-cmake-modules base-devel
  yay -S qt5-tools
  ```
</details>
<details>
<summary>OpenSUSE - by https://github.com/mathiasgredal, https://github.com/Richardsause, and https://github.com/aaronkirschen</summary>

- Plasma 6 (by https://github.com/aaronkirschen)
  ```bash
  sudo zypper in git cmake gcc-c++ kf6-kconfigwidgets-devel kf6-kcmutils-devel kwin6-devel kf6-kwindowsystem-devel qt6-quick-devel qt6-core-private-devel
  ``` 
- Plasma 5 (by https://github.com/mathiasgredal, https://github.com/Richardsause)
  ```bash
  sudo zypper install git cmake gcc-c++ extra-cmake-modules libqt5-qttools-devel kconfigwidgets-devel kwindowsystem-devel kguiaddons-devel ki18n-devel knotifications-devel kwin5-devel libQt5Gui-devel libQt5OpenGL-devel libepoxy-devel libqt5-qtnetworkauth-devel
  ```

</details>
<details>
<summary>Void - by https://github.com/lay-by and https://github.com/tre-flip</summary>

  ```bash
  xbps-install git cmake make extra-cmake-modules gettext-devel kwin-devel kf6-kconfigwidgets-devel kf6-kcmutils-devel
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

> [!Note]
> If you are building for X11, use the command `cmake .. -DKWIN_X11=ON` instead of `cmake ..`

> [!Note]
> When building for KDE Linux (https://community.kde.org/KDE_Linux) or other immutable distributions that do not provide tools like `rpm-ostree`, you cannot write to system paths such as `/usr`. To deploy a plugin in such environments, you need to build it into a user overlay and activate it using `systemd-sysext`.
>
> To ensure the plugin is installed under the correct directory layout expected by KWin, use
> ```
> cmake .. \
>   -DCMAKE_INSTALL_PREFIX=~/kde/usr \
>   -DKDE_INSTALL_PLUGINDIR=lib/qt6/plugins
> ```
> instead of `cmake ..`, and also use `make install` **instead of** `sudo make install`.
>
> Once built, you can prepare the overlay by placing it under `~/kde` and linking it into `/var/lib/extensions/kde`, where `systemd-sysext` expects extension roots to be located. The following command automates that:
>
> ```bash
> sh ../tools/deploy-to-overlay.sh
> ```
> This way, the plugin is installed in a form that `systemd-sysext` can recognize and load, while still preserving the integrity of the immutable root filesystem.

# How to load or unload the effect

To activate the effect, you can now log out and log back in, or run the command below inside the `build` directory:

```bash
sh ../tools/load.sh
```

To fully uninstall the effect, run the following commands inside the `build` directory:

```bash
sh ../tools/unload.sh
sudo make uninstall
```

# How to auto-install after KWin update

After each KWin package update, the effect becomes incompatible. So it won't load without a rebuild.

As long as the effect is not part of the KWin yet (being discussed
[here](https://invent.kde.org/plasma/kwin/-/issues/198)), you can automate the re-installation by running the command
below inside the `build` directory:

```bash
sh ../tools/install-autorun-test.sh
```

The command above adds a `.desktop` file inside the `autorun` directory which checks if the effect is still compatible,
If it is incompatible, the script will automatically rebuild and reinstall the effect.

> [!NOTE]
> The script uses `qdbus` to show a progress bar. On Plasma 6, it is not installed by default. You need to manually install the package `qtchooser`.

# How to adjust its settings

You can change the corner radius, outlines, exclusions, and more in:

> System Settings → Workspace Behavior → Desktop Effects → Rounded Corners

# Extra Tips

## How to add more languages to the translation

Translations are stored in `po/<lang>/kcmcorners.po`. One can copy from other languages and start editing it in a different subdirectory of `po`.

If the `kcm.ui` changes, a new language template `.pot` can be generated by running the script `tools/generate-messages.sh`.

## How to add shadow to windows without decoration (like Steam)

You can add shadows for specific windows using the hack below. I don't know how to enforce it in my code.

1. In [ System settings ] → [ Window management ] → [ Window rules ] → [ Appearance & Fixes ]:

   **Add [steam] and set [ No titlebar ] and frame to [ No ]**

2. In [ System settings ] → [ Application Style ] → [ Window decoration ] → [ Breeze theme setting ] → [ Window specific overrides ]:

   **Add [steam] and set [ Hide Window title bar ] to [ Yes ].**

After that, the Steam window gets its shadows back.

## How to add debug messages

When troubleshooting or reporting an issue, it might be useful to enable Debug logs during the build time using:

```bash
cmake .. --DCMAKE_BUILD_TYPE=Debug
cmake --build . -j
```

You may also need to add the `default.debug=true` rule in `~/.config/QtProject/qtlogging.ini`. The example of the file content is:

```ini
[Rules]
*.debug=false
qt.qpa.xcb.xcberror.warning=false
default.debug=true
```

After the installation and loading the effect, debug messages would appear in `journalctl`:

```bash
journalctl -f | grep kwin
```

or have some colorful logs with

```bash
sh ../tools/show-kwin-logs.sh
```
