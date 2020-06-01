ARG project

FROM server:builder AS builder

FROM scratch AS skyrim
COPY --from=builder /home/server/Build/bin/x64/SkyrimTogetherServer /SkyrimTogetherServer
ENTRYPOINT ["/SkyrimTogetherServer"]

FROM scratch AS fallout4
COPY --from=builder /home/server/Build/bin/x64/FalloutTogetherServer /FalloutTogetherServer
ENTRYPOINT ["/FalloutTogetherServer"]

FROM ${project} AS final

FROM final
EXPOSE 10578/udp
