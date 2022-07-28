@echo off

docker build . -t tiltedphoques/st-reborn-server:v1.28.0-84
mkdir build\linux\x64
docker run --rm --entrypoint /bin/sh tiltedphoques/st-reborn-server:v1.28.0-84 -c "cat /home/server/SkyrimTogetherServer.debug" > ./build/linux/x64/SkyrimTogetherServer.debug
docker run --rm --entrypoint /bin/sh tiltedphoques/st-reborn-server:v1.28.0-84 -c "cat /home/server/SkyrimTogetherServer" > ./build/linux/x64/SkyrimTogetherServer
docker run --rm --entrypoint /bin/sh tiltedphoques/st-reborn-server:v1.28.0-84 -c "cat /home/server/libSTServer.so" > ./build/linux/x64/libSTServer.so
docker run --rm --entrypoint /bin/sh tiltedphoques/st-reborn-server:v1.28.0-84 -c "cat /home/server/libSTServer.debug" > ./build/linux/x64/libSTServer.debug
timeout /t 30 /nobreak
