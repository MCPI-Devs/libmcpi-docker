#!/bin/sh

set -e

# Docker Messes With SetGID
chmod -R g-s debian

# Clean out Directory
rm -rf out
mkdir -p out/deb

# Generate DEB
dpkg -b debian out/deb

# Export Libraries
mkdir -p out/lib

# Copy Headers
cp -r mods/include out/lib/include

# Copy Shared Library
IMG_ID="$(docker create thebrokenrail/minecraft-pi)"
docker cp "${IMG_ID}":/app/minecraft-pi/mods/. ./out/lib/.
docker rm -v "${IMG_ID}"
