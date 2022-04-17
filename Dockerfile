FROM ubuntu:20.04 AS builder

RUN apt update && \
    apt install software-properties-common -y && \
    add-apt-repository 'deb http://mirrors.kernel.org/ubuntu hirsute main universe' -y && \
    apt update && \
    apt install libssl-dev curl p7zip-full p7zip-rar zip unzip zlib1g-dev -y

RUN curl -fsSL https://xmake.io/shget.text > getxmake.sh && chmod +x getxmake.sh && ./getxmake.sh && \
    apt remove gcc-10 g++-10 -y && \
    apt install gcc-11 g++-11 -y && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 110 --slave /usr/bin/g++ g++ /usr/bin/g++-11 --slave /usr/bin/gcov gcov /usr/bin/gcov-11

WORKDIR /home/server

COPY ./modules ./modules
COPY ./Libraries ./Libraries
COPY xmake.lua xmake.lua
COPY ./.git ./.git
COPY ./Code ./Code
COPY ./tmprepo ./tmprepo

RUN export XMAKE_ROOTDIR="/root/.local/bin" && \
export PATH="$XMAKE_ROOTDIR:$PATH" && \
export XMAKE_ROOT=y && \
xmake config -y && \
xmake -j8

FROM ubuntu:20.04 AS skyrim

RUN apt update && \
    apt install software-properties-common -y && \
    add-apt-repository 'deb http://mirrors.kernel.org/ubuntu hirsute main universe' -y && \
    apt update && apt upgrade -y && \
    apt remove software-properties-common -y && \
    apt autoremove -y

COPY --from=builder /home/server/build/linux/x64/release/SkyrimTogetherServer /home/server/SkyrimTogetherServer
WORKDIR /home/server
ENTRYPOINT ["./SkyrimTogetherServer"]

EXPOSE 10578/udp
