FROM server:base

ARG make_jobs=8

WORKDIR /home/server

COPY ./Build ./Build
COPY ./Code ./Code
COPY ./Libraries ./Libraries

RUN ln  -s -f /usr/local/bin/premake5 ./Build/premake5 && \
    ln  -s -f /usr/local/bin/protoc ./Libraries/TiltedConnect/Build/protoc && \
    cd Build && \
    ./MakeGMakeProjects.sh && \
    cd projects && \
    make config=skyrim_x64 -j $make_jobs && \
    make config=fallout4_x64 -j $make_jobs
