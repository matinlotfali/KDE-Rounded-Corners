FROM kdeneon/plasma:unstable
ENV DEBIAN_FRONTEND=noninteractive
RUN sudo apt update && sudo apt install -y cmake extra-cmake-modules g++  && sudo apt clean
RUN sudo apt update && sudo apt install -y gettext qttools5-dev libqt5x11extras5-dev libkf5configwidgets-dev \
        libkf5globalaccel-dev libkf5notifications-dev kwin-dev && sudo apt clean
RUN sudo apt update && sudo apt install -y file dpkg-dev  && sudo apt clean