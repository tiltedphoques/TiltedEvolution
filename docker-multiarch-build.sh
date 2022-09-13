#build the builder for both x86_64 and arm64 platforms
docker buildx build --platform linux/amd64,linux/arm64 -f Dockerfile.builder -t tiltedphoques/multiarch-builder:latest --push .

#build the server for both x86_64 and arm64 platforms
docker buildx build --platform linux/amd64,linux/arm64 -f Dockerfile -t tiltedphoques/str-reborn-server:latest --push .