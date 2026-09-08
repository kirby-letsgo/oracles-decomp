FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends mono-complete xvfb libopenal1 libgl1 libglu1-mesa libgtk2.0-0 libsdl2-2.0-0 lsb-release ca-certificates xdotool imagemagick x11-apps libsdl2-dev libopenal-dev libgl1-mesa-dri mesa-utils liblua5.4-0 liblua5.4-dev && rm -rf /var/lib/apt/lists/*
WORKDIR /bizhawk
COPY BizHawk-linux.tar.gz /tmp/
RUN tar -xzf /tmp/BizHawk-linux.tar.gz -C /bizhawk && rm /tmp/BizHawk-linux.tar.gz
WORKDIR /bizhawk/BizHawk-2.11.1-linux-x64
ENV HOME=/root
CMD ["bash"]
RUN useradd -m -u 1500 biz && chown -R biz /bizhawk
USER biz
ENV HOME=/home/biz
