FROM ubuntu:20.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt update && apt install -y cmake extra-cmake-modules g++  && apt clean
RUN apt update && apt install -y gettext qttools5-dev libqt5x11extras5-dev libkf5configwidgets-dev \
        libkf5globalaccel-dev libkf5notifications-dev kwin-dev && apt clean
RUN apt update && apt install -y file dpkg-dev  && apt clean