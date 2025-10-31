FROM debian:12 AS builder

ENV DEBIAN_FRONTEND=noninteractive
ENV XMAKE_ROOT=y
SHELL ["/bin/bash", "-c"]

# Get packages and xmake

RUN apt-get update && apt-get install -y --no-install-recommends build-essential curl pkg-config git ca-certificates unzip libssl-dev && \
    rm -rf /var/lib/apt/lists/* && \
    curl -fsSL https://xmake.io/shget.text | bash

# Copy source and build

WORKDIR /src
COPY . /src

RUN source ~/.xmake/profile && \
    xmake config -y -m release && xmake -y && xmake install -y -o package

# Actual server runtime image

FROM gcr.io/distroless/cc-debian12 AS runtime

WORKDIR /st-server

COPY --from=builder \
    /src/package/lib/libSTServer.so \
    /src/package/bin/crashpad_handler \
    /src/package/bin/SkyrimTogetherServer \
    /st-server/

# Run
ENTRYPOINT ["./SkyrimTogetherServer"]
EXPOSE 10578/udp
