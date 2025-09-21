FROM debian:13

# Install dependencies
RUN apt-get update && apt-get -y --no-install-recommends full-upgrade
RUN apt-get install -y --no-install-recommends \
        bash-completion \
        locales

# Install Locales
RUN sed -i '/en_US.UTF-8/s/^# //g' /etc/locale.gen && \
    locale-gen
ENV LANG=en_US.UTF-8
ENV LANGUAGE=en_US:en
ENV LC_ALL=en_US.UTF-8

# Install Plasma Desktop
RUN apt-get install -y --no-install-recommends \
        plasma-desktop \
        plasma-workspace \
        systemsettings \
        kwin-x11 \
        konsole \
        dolphin

# Install KDE-Rounded-Corners dependencies
RUN apt-get install -y --no-install-recommends \
        cmake \
        make \
        g++ \
        gettext \
        extra-cmake-modules \
        qt6-base-private-dev \
        qt6-base-dev-tools \
        libkf6configwidgets-dev \
        libkf6kcmutils-dev \
        kwin-dev

# Build and Install KDE-Rounded-Corners
RUN --mount=type=bind,source=.,target=/workspace \
    --mount=type=cache,target=/dockerbuild \
    cd /dockerbuild && \
    cmake /workspace -DCMAKE_BUILD_TYPE=Debug && \
    cmake --build . -j && \
    cmake --install .

CMD ["startplasma-x11 | grep ShapeCorners"]