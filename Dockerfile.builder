FROM ubuntu:20.04 AS builder

RUN apt update && \
    apt install software-properties-common -y && \
    add-apt-repository universe -y && \
    apt update && \
    apt install libssl-dev curl p7zip-full p7zip-rar zip unzip zlib1g-dev wget -y && \
    curl -fsSL https://xmake.io/shget.text > getxmake.sh && chmod +x getxmake.sh && ./getxmake.sh && \
    wget ftp://ftp.fu-berlin.de/unix/languages/gcc/releases/gcc-12.1.0/gcc-12.1.0.tar.xz && \
    tar xf gcc-12.1.0.tar.xz && \
    rm -f gcc-12.1.0.tar.xz && \
    cd gcc-12.1.0 && \
    contrib/download_prerequisites && \
    mkdir -p obj && \
    cd obj && \
    ../configure --enable-languages=c,c++ --disable-multilib && \
    make -j 4 && \
    make install && \
    cd ../.. && \
    rm -rf gcc-12.1.0