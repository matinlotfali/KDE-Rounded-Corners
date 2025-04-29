Summary:        KDE KWin effect to round the corners of windows
Name:           kwin-effect-roundcorners
Version:        0.7.1
Release:        %{autorelease}
License:        GPL-3.0
URL:            https://github.com/matinlotfali/KDE-Rounded-Corners
Source0:        https://github.com/matinlotfali/KDE-Rounded-Corners/archive/refs/heads/master.tar.gz

# Build requirements
BuildRequires:  cmake
BuildRequires:  gcc-c++
BuildRequires:  extra-cmake-modules
BuildRequires:  kf6-kconfigwidgets-devel
BuildRequires:  kf6-kcmutils-devel
BuildRequires:  kf6-ki18n-devel
BuildRequires:  kf6-kwindowsystem-devel
BuildRequires:  libepoxy-devel
BuildRequires:  libxcb-devel
BuildRequires:  wayland-devel
%if %{defined suse_version}
BuildRequires:  kwin6-devel
BuildRequires:  qt6-core-private-devel
BuildRequires:  qt6-quick-devel
BuildRequires:  kf6-rpm-macros
%else
BuildRequires:  kwin-devel
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qtbase-private-devel
BuildRequires:  kf6-filesystem
%endif

%description
KDE Rounded Corners is a desktop effect for KWin that smoothly rounds
the corners of all windows and optionally adds an outline, with
minimal impact on performance.

%prep
%setup -q -n KDE-Rounded-Corners-master

%build
%cmake_kf6
%cmake_build

%install
%cmake_install

%files
%license LICENSE
%doc README.md
%{_kf6_qtplugindir}/kwin/effects/configs/kwin_shapecorners_config.so
%{_kf6_qtplugindir}/kwin/effects/plugins/kwin4_effect_shapecorners.so
%{_kf6_datadir}/kwin/shaders/shapecorners.frag
%{_kf6_datadir}/kwin/shaders/shapecorners_core.frag
%{_kf6_datadir}/locale/*/LC_MESSAGES/kcmcorners.mo*

%changelog
* Thu Apr 24 2025 Matin Lotfaliei <matinlotfali@gmail.com> - 0.7.1-1
- Initial RPM packaging of KDE-Rounded-Corners
