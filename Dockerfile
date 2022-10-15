FROM tiltedphoques/multiarch-builder:latest as builder

ARG REPO=https://github.com/tiltedphoques/TiltedEvolution.git
ARG BRANCH=master

WORKDIR /home/builder

ENV XMAKE_ROOT=y

RUN git clone --recursive -b ${BRANCH} ${REPO} ./str && \
    cd str && xmake config -m release -y && xmake -y && xmake install -o package -y


# Building for x86_64
FROM builder as amd64builder

RUN cp /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30 /home/builder/libstdc++.so.6 2>/dev/null || :


# Building for arm64/v8
FROM builder as arm64builder

RUN cp /usr/lib/aarch64-linux-gnu/libstdc++.so.6.0.30 /home/builder/libstdc++.so.6 2>/dev/null || :


# Intermediate image that has the library specific to our $TARGETARCH
FROM ${TARGETARCH}builder as intermediate
# If a user has built without buildx, attempt to save them
RUN if [ "${TARGETARCH}" = "" ]; then export LIBFILE="/usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30"; if [ ! -e ${LIBFILE} ]; then export LIBFILE=/usr/lib/aarch64-linux-gnu/libstdc++.so.6.0.30; fi ; cp ${LIBFILE} /home/builder/libstdc++.so.6; fi


# Build actual server image
FROM ubuntu:22.04

COPY --from=intermediate /home/builder/str/package/lib/libSTServer.so /home/server/libSTServer.so
COPY --from=intermediate /home/builder/str/package/bin/crashpad_handler /home/server/crashpad_handler
COPY --from=intermediate /home/builder/str/package/bin/SkyrimTogetherServer /home/server/SkyrimTogetherServer

COPY --from=intermediate /home/builder/libstdc++.so.6 /home/server/libstdc++.so.6

WORKDIR /home/server
ENTRYPOINT ["./SkyrimTogetherServer"]

EXPOSE 10578/udp
