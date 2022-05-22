FROM tiltedphoques/builder AS builder

WORKDIR /home/server

COPY ./modules ./modules
COPY ./Libraries ./Libraries
COPY xmake.lua xmake.lua
COPY ./.git ./.git
COPY ./Code ./Code

RUN export XMAKE_ROOTDIR="/root/.local/bin" && \
export PATH="$XMAKE_ROOTDIR:$PATH" && \
export XMAKE_ROOT=y && \
xmake config -y && \
xmake -j8 && \
objcopy --only-keep-debug /home/server/build/linux/x64/release/SkyrimTogetherServer /home/server/build/linux/x64/release/SkyrimTogetherServer.debug && \
objcopy --only-keep-debug /home/server/build/linux/x64/release/libSTServer.so /home/server/build/linux/x64/release/libSTServer.debug

RUN export XMAKE_ROOTDIR="/root/.local/bin" && \
export PATH="$XMAKE_ROOTDIR:$PATH" && \
export XMAKE_ROOT=y && \
xmake install -o package

FROM ubuntu:20.04 AS skyrim

RUN apt update && \
    apt install software-properties-common -y && \
    add-apt-repository 'deb http://mirrors.kernel.org/ubuntu hirsute main universe' -y && \
    apt update && apt upgrade -y && \
    apt remove software-properties-common -y && \
    apt autoremove -y

COPY --from=builder /home/server/package/lib/libSTServer.so /home/server/libSTServer.so
COPY --from=builder /home/server/package/bin/SkyrimTogetherServer /home/server/SkyrimTogetherServer
COPY --from=builder /home/server/package/bin/crashpad_handler /home/server/crashpad_handler
COPY --from=builder /home/server/build/linux/x64/release/libSTServer.debug /home/server/libSTServer.debug
COPY --from=builder /home/server/build/linux/x64/release/SkyrimTogetherServer.debug /home/server/SkyrimTogetherServer.debug
WORKDIR /home/server
ENTRYPOINT ["./SkyrimTogetherServer"]

EXPOSE 10578/udp
