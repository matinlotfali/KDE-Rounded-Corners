Summary:        KDE KWin effect to round the corners of windows
Name:           kwin-effect-roundcorners
Version:        0.7.1
Release:        1%{?dist}
License:        GPL-3.0
Vendor:         https://github.com/matinlotfali
URL:            https://github.com/matinlotfali/KDE-Rounded-Corners
Source0:        https://github.com/matinlotfali/KDE-Rounded-Corners/archive/refs/heads/master.tar.gz

# Build requirements
BuildRequires:  cmake >= 3.20
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
%else
BuildRequires:  kwin-devel
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qtbase-private-devel
%endif

%description
KDE Rounded Corners is a desktop effect for KWin that smoothly rounds
the corners of all windows and optionally adds an outline, with
minimal impact on performance.

%prep
%setup -q -n KDE-Rounded-Corners-master

%build
mkdir -p build
cd build
cmake .. \
    -DCMAKE_INSTALL_PREFIX=%{_prefix} \
    -DCMAKE_INSTALL_LIBDIR=%{_libdir}
make %{?_smp_mflags}

%install
cd build
make install DESTDIR=%{buildroot}

%files
%license LICENSE
%doc README.md
%{_libdir}/qt6/plugins/kwin/effects/configs/kwin_shapecorners_config.so
%{_libdir}/qt6/plugins/kwin/effects/plugins/kwin4_effect_shapecorners.so
%{_datadir}/kwin/shaders/shapecorners.frag
%{_datadir}/kwin/shaders/shapecorners_core.frag
%{_datadir}/locale/*/LC_MESSAGES/kcmcorners.mo*

%changelog
* Thu Apr 24 2025 Matin Lotfaliei <matinlotfali@gmail.com> - 0.7.1-1
- Initial RPM packaging of KDE-Rounded-Corners
