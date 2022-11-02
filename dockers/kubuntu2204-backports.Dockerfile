FROM matinlotfali/kubuntu2204
ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install -y software-properties-common && apt clean
RUN add-apt-repository -y ppa:kubuntu-ppa/backports-extra && apt clean
RUN apt update && apt -y dist-upgrade && apt clean