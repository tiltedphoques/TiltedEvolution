FROM gcc:12.1.0

# install packages
RUN \
    apt update \
    && DEBIAN_FRONTEND=noninteractive apt install -y --no-install-recommends \
        cmake \
        sudo

# switch to ruki user
RUN groupadd -r ruki && useradd -m -r -g ruki ruki
RUN chown -R ruki:ruki /home
RUN echo "root:0000" | chpasswd
RUN echo "ruki:0000" | chpasswd
RUN echo "ruki ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers
USER ruki

# install xmake
RUN cd /tmp/ && git clone --depth=1 "https://github.com/xmake-io/xmake.git" --recursive xmake && cd xmake && ./scripts/get.sh __local__
RUN rm -rf /tmp/xmake
