#set up buildx builder
docker buildx create --name str-multiarch --use

#clean up qemu for some reason
docker run --rm --privileged multiarch/qemu-user-static --reset -p yes

#build the builder for both x86_64 and arm64 platforms
docker buildx build --platform linux/amd64,linux/arm64 -f Dockerfile.builder -t tiltedphoques/multiarch-builder:latest --push .

#build the server for both x86_64 and arm64 platforms
docker buildx build --platform linux/amd64,linux/arm64 -f Dockerfile -t tiltedphoques/st-reborn-server:latest --push .

#remove buildx builder
docker buildx rm str-multiarch