ARG arch=x86_64
ARG build_dir=x86_64

FROM tiltedphoques/builder AS builder

ARG arch
ARG build_dir

WORKDIR /home/server

COPY ./modules ./modules
COPY ./Libraries ./Libraries
COPY xmake.lua xmake.lua
COPY ./.git ./.git
COPY ./Code ./Code

RUN ~/.local/bin/xmake f -y -vD
RUN ~/.local/bin/xmake -j`nproc` -vD

RUN objcopy --only-keep-debug /home/server/build/linux/${build_dir}/release/SkyrimTogetherServer /home/server/build/linux/${build_dir}/release/SkyrimTogetherServer.debug
RUN objcopy --only-keep-debug /home/server/build/linux/${build_dir}/release/libSTServer.so /home/server/build/linux/${build_dir}/release/libSTServer.debug

RUN ~/.local/bin/xmake install -o package

FROM ubuntu:20.04 AS skyrim

ARG arch
ARG build_dir

RUN apt update && apt install libssl1.1

COPY --from=builder /usr/local/lib64/libstdc++.so.6.0.30 /lib/${arch}-linux-gnu/libstdc++.so.6

# Now copy the actual bins
COPY --from=builder /home/server/package/lib/libSTServer.so /home/server/libSTServer.so
COPY --from=builder /home/server/package/bin/SkyrimTogetherServer /home/server/SkyrimTogetherServer
COPY --from=builder /home/server/package/bin/crashpad_handler /home/server/crashpad_handler
COPY --from=builder /home/server/build/linux/${build_dir}/release/libSTServer.debug /home/server/libSTServer.debug
COPY --from=builder /home/server/build/linux/${build_dir}/release/SkyrimTogetherServer.debug /home/server/SkyrimTogetherServer.debug
WORKDIR /home/server
ENTRYPOINT ["./SkyrimTogetherServer"]

EXPOSE 10578/udp