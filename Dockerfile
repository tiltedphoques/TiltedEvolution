# Building the server:
#   x64:   $ docker build -t imagename .
#   arm64: $ docker build --platform linux/arm64 -t imagename .
#   multi-platform (build and push):
#     $ docker buildx create --name st-server-multiarch --use
#     $ docker buildx build --platform linux/amd64,linux/arm64 -t imagename:tag --push .

FROM --platform=$TARGETOS/$TARGETARCH debian:12 AS builder

ENV DEBIAN_FRONTEND=noninteractive
ENV XMAKE_ROOT=y
SHELL ["/bin/bash", "-c"]

# Get packages and xmake

RUN apt-get update && apt-get install -y --no-install-recommends build-essential curl pkg-config git ca-certificates unzip libssl-dev && \
    rm -rf /var/lib/apt/lists/* && \
    curl -fsSL https://xmake.io/shget.text | bash

# Copy only xmake config files & submodules
WORKDIR /src

COPY xmake.lua ./
COPY modules/version.lua modules/
COPY Libraries/xmake.lua Libraries/
COPY Code/xmake.lua Code/
# 🤪?
COPY Code/admin/xmake.lua Code/admin/
COPY Code/admin_protocol/xmake.lua Code/admin_protocol/
COPY Code/base/xmake.lua Code/base/
COPY Code/client/xmake.lua Code/client/
COPY Code/common/xmake.lua Code/common/
COPY Code/components/xmake.lua Code/components/
COPY Code/components/console/xmake.lua Code/components/console/
COPY Code/components/crash_handler/xmake.lua Code/components/crash_handler/
COPY Code/components/es_loader/xmake.lua Code/components/es_loader/
COPY Code/components/imgui/xmake.lua Code/components/imgui/
COPY Code/components/resources/xmake.lua Code/components/resources/
COPY Code/encoding/xmake.lua Code/encoding/
COPY Code/immersive_elf/xmake.lua Code/immersive_elf/
COPY Code/immersive_launcher/xmake.lua Code/immersive_launcher/
COPY Code/server/xmake.lua Code/server/
COPY Code/server_runner/xmake.lua Code/server_runner/
COPY Code/tests/xmake.lua Code/tests/
COPY Code/tp_process/xmake.lua Code/tp_process/

COPY Libraries/ Libraries/

RUN source ~/.xmake/profile && \
    xmake config -y -m release

# Copy source and build
COPY . /src

RUN source ~/.xmake/profile && \
    xmake -y && \
    xmake install -y -o package

# Actual server runtime image; distroless for small footprint

FROM --platform=$TARGETOS/$TARGETARCH gcr.io/distroless/cc-debian12 AS runtime

WORKDIR /st-server

COPY --from=builder \
    /src/package/lib/libSTServer.so \
    /src/package/bin/crashpad_handler \
    /src/package/bin/SkyrimTogetherServer \
    /st-server/

ENTRYPOINT ["./SkyrimTogetherServer"]
EXPOSE 10578/udp
