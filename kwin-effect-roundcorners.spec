Summary:        KDE KWin effect to round the corners of windows
Name:           kwin-effect-roundcorners
License:        GPL-3.0
URL:            https://github.com/matinlotfali/KDE-Rounded-Corners
Source0:        %{URL}/archive/refs/heads/master.tar.gz
Version:        0.7.2

%if %{defined suse_version}
BuildRequires:  qt6-core-private-devel
BuildRequires:  qt6-quick-devel
%global kwin_pkg_name    kwin6
%global kwin_version     %(zypper info -y kwin6 2>/dev/null | awk '$1=="Version" {print $3}')
%global cmake_kf6        mkdir -p build; cd build; cmake .. -DCMAKE_INSTALL_PREFIX=%{_prefix} -DCMAKE_INSTALL_LIBDIR=%{_libdir}
%global cmake_build      make %{?_smp_mflags}
%global cmake_install    cd build; make install DESTDIR=%{buildroot}
%global _kf6_qtplugindir %{_libdir}/qt6/plugins
%global _kf6_datadir     %{_datadir}

%else
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qtbase-private-devel
BuildRequires:  kf6-rpm-macros
%global kwin_pkg_name kwin
%global kwin_version  %(dnf info -y kwin 2>/dev/null | awk '$1=="Version" {print $3}')

%endif

Release:        %{kwin_version}%{?dist}
Requires:       %{kwin_pkg_name} = %{kwin_version}
BuildRequires:  %{kwin_pkg_name}-devel
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
%if %{defined suse_version}
* Thu Apr 24 2025 Matin Lotfaliei <matinlotfali@gmail.com> - 0.7.1-1
- Initial RPM packaging of KDE-Rounded-Corners
%else
%autochangelog
%endif
