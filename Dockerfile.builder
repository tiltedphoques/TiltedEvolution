FROM ubuntu:22.04

WORKDIR /home/builder

RUN apt-get update

RUN apt-get install -y --no-install-recommends cmake \
    unzip \
    git \
    gcc-12 \
    g++-12 \
    build-essential \
    ca-certificates \
    curl &&\
    curl -fsSL https://xmake.io/shget.text | bash &&\
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 110 --slave /usr/bin/g++ g++ /usr/bin/g++-12 --slave /usr/bin/gcov gcov /usr/bin/gcov-12 && \
    rm -rf /var/lib/apt/lists/*
