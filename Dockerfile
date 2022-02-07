ARG project

FROM server:builder AS builder

FROM ubuntu AS template
RUN apt update && \
    apt-get install software-properties-common unzip -y && \
    add-apt-repository -y ppa:ubuntu-toolchain-r/test && \
    apt update && \
    apt install gcc-11 g++-11 libssl-dev -y && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 110 --slave /usr/bin/g++ g++ /usr/bin/g++-11 --slave /usr/bin/gcov gcov /usr/bin/gcov-11


FROM template AS skyrim
COPY --from=builder /home/server/build/linux/x64/release/SkyrimTogetherServer /SkyrimTogetherServer
ENTRYPOINT ["/SkyrimTogetherServer"]

FROM template AS fallout4
COPY --from=builder /home/server/build/linux/x64/release/FalloutTogetherServer /FalloutTogetherServer
ENTRYPOINT ["/FalloutTogetherServer"]

FROM ${project} AS final

FROM final
EXPOSE 10578/udp
