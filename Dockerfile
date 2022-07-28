ARG arch=x86_64

FROM tiltedphoques/builder:${arch} AS builder

ARG arch

WORKDIR /home/server

RUN apt update && \
apt install cmake -y

RUN apt install -y sudo

# switch to ruki user
RUN groupadd -r ruki && useradd -r -g ruki ruki
RUN mkdir /home/ruki
RUN chown -R ruki:ruki /home
RUN echo "root:0000" | chpasswd
RUN echo "ruki:0000" | chpasswd
RUN echo "ruki ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers
USER ruki

# install xmake
RUN cd /tmp/ && git clone --depth=1 "https://github.com/xmake-io/xmake.git" --recursive xmake && cd xmake && ./scripts/get.sh __local__
RUN rm -rf /tmp/xmake

COPY ./modules ./modules
COPY ./Libraries ./Libraries
COPY xmake.lua xmake.lua
COPY ./.git ./.git
COPY ./Code ./Code

RUN ~/.local/bin/xmake f -y -vD
RUN ~/.local/bin/xmake -j`nproc` -vD

RUN objcopy --only-keep-debug /home/server/build/linux/${arch}/release/SkyrimTogetherServer /home/server/build/linux/${arch}/release/SkyrimTogetherServer.debug
RUN objcopy --only-keep-debug /home/server/build/linux/${arch}/release/libSTServer.so /home/server/build/linux/${arch}/release/libSTServer.debug

RUN ~/.local/bin/xmake install -o package

FROM ubuntu:20.04 AS skyrim

ARG arch

RUN apt update && apt install libssl1.1

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