ARG project

FROM muslcc/x86_64:x86_64-linux-musl AS builder

RUN apk add --no-cache --update autoconf automake linux-headers p7zip curl tar unzip cmake bash
RUN curl -fsSL https://xmake.io/shget.text > getxmake.sh && chmod +x getxmake.sh && ./getxmake.sh && apk del g++ gcc libc-dev musl-dev && ln -s /bin/gcc-11.2.1 /usr/bin/gcc

WORKDIR /home/server

COPY ./modules ./modules
COPY ./Libraries ./Libraries
COPY xmake.lua xmake.lua
COPY ./.git ./.git
COPY ./Code ./Code

RUN export XMAKE_ROOTDIR="/root/.local/bin" && \
export PATH="$XMAKE_ROOTDIR:$PATH" && \
export XMAKE_ROOT=y && \
xmake config -y --ldflags="-static" && \
xmake -y -j8

FROM scratch AS skyrim
COPY --from=builder /home/server/build/linux/x64/release/SkyrimTogetherServer /SkyrimTogetherServer
ENTRYPOINT ["/SkyrimTogetherServer"]

FROM scratch AS fallout4
COPY --from=builder /home/server/build/linux/x64/release/FalloutTogetherServer /FalloutTogetherServer
ENTRYPOINT ["/FalloutTogetherServer"]

FROM ${project} AS final

FROM final

EXPOSE 10578/udp
