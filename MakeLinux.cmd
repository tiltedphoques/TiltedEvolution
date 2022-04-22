@echo off

docker build . -t skyrim_build
docker run --rm --entrypoint /bin/sh skyrim_build -c "cat /home/server/SkyrimTogetherServer.debug" > ./build/linux/x64/SkyrimTogetherServer.debug
docker run --rm --entrypoint /bin/sh skyrim_build -c "cat /home/server/SkyrimTogetherServer" > ./build/linux/x64/SkyrimTogetherServer
docker run --rm --entrypoint /bin/sh skyrim_build -c "cat /home/server/libSTServer.so" > ./build/linux/x64/libSTServer.so
docker run --rm --entrypoint /bin/sh skyrim_build -c "cat /home/server/libSTServer.debug" > ./build/linux/x64/libSTServer.debug
timeout /t 30 /nobreak
