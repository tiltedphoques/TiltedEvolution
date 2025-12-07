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

COPY Libraries/ Libraries/

RUN --mount=type=cache,target=/root/.xmake/packages \
    source ~/.xmake/profile && \
    xmake config -y -m release

# Copy source and build
COPY . /src

RUN --mount=type=cache,target=/root/.xmake/packages \
    --mount=type=cache,target=/root/.xmake/cache \
    source ~/.xmake/profile && \
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
