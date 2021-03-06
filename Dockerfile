FROM arm64v8/debian:bullseye

RUN dpkg --add-architecture armhf

RUN \
    apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y libglvnd-dev:armhf libsdl1.2-dev:armhf libx11-dev:armhf build-essential zlib1g-dev:armhf git cmake curl gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf libfreeimage-dev:armhf libglfw3-dev:armhf xinput:armhf libxfixes-dev:armhf

RUN ln -s /usr/lib/arm-linux-gnueabihf/libGLESv2.so.2 /usr/lib/libGLESv2.so
RUN ln -s /usr/lib/arm-linux-gnueabihf/libEGL.so.1 /usr/lib/libEGL.so

ADD ./build /app/build

WORKDIR /app

RUN ./build/download-minecraft-pi.sh

RUN ./build/build-libpng12.sh

ADD . /app

RUN ./build/build-mods.sh

WORKDIR ./minecraft-pi

ENTRYPOINT ./launcher
