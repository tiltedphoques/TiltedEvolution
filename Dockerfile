ARG arch=x64

FROM tiltedphoques/builder:${arch} AS builder

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
objcopy --only-keep-debug /home/server/build/linux/${arch}/release/SkyrimTogetherServer /home/server/build/linux/${arch}/release/SkyrimTogetherServer.debug && \
objcopy --only-keep-debug /home/server/build/linux/${arch}/release/libSTServer.so /home/server/build/linux/${arch}/release/libSTServer.debug

RUN export XMAKE_ROOTDIR="/root/.local/bin" && \
export PATH="$XMAKE_ROOTDIR:$PATH" && \
export XMAKE_ROOT=y && \
xmake install -o package

FROM ubuntu:20.04 AS skyrim

# We copy it twice since we can't really tell the arch from Dockerfile :(
COPY --from=builder /usr/local/lib64/libstdc++.so.6.0.30 /lib/x86_64-linux-gnu/libstdc++.so.6
COPY --from=builder /usr/local/lib64/libstdc++.so.6.0.30 /lib/aarch64-linux-gnu/libstdc++.so.6

# Now copy the actual bins
COPY --from=builder /home/server/package/lib/libSTServer.so /home/server/libSTServer.so
COPY --from=builder /home/server/package/bin/SkyrimTogetherServer /home/server/SkyrimTogetherServer
COPY --from=builder /home/server/package/bin/crashpad_handler /home/server/crashpad_handler
COPY --from=builder /home/server/build/linux/${arch}/release/libSTServer.debug /home/server/libSTServer.debug
COPY --from=builder /home/server/build/linux/${arch}/release/SkyrimTogetherServer.debug /home/server/SkyrimTogetherServer.debug
WORKDIR /home/server
ENTRYPOINT ["./SkyrimTogetherServer"]

EXPOSE 10578/udp
