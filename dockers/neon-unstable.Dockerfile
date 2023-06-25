FROM kdeneon/plasma:unstable
ENV DEBIAN_FRONTEND=noninteractive
RUN sudo apt update
RUN sudo apt dist-upgrade -y
RUN sudo apt install -y cmake extra-cmake-modules g++
RUN sudo apt install -y kwin-dev
RUN sudo apt install -y file dpkg-dev