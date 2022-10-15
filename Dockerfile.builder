FROM ubuntu:22.04

WORKDIR /home/builder

RUN apt update && apt install \
    cmake \
    unzip \
    git \
    gcc-12 \
    g++-12 \
    build-essential \
    ca-certificates \
    curl \
    --no-install-recommends -y && \
    git clone --recursive https://github.com/xmake-io/xmake.git ./xmake && \
    cd xmake && \
    make build && \
    make install && \
    cd .. && rm -rf xmake/ && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 110 --slave /usr/bin/g++ g++ /usr/bin/g++-12 --slave /usr/bin/gcov gcov /usr/bin/gcov-12 && \
    rm -rf /var/lib/apt/lists/*