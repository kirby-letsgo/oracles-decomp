#!/bin/sh
# Packages for building oracles-native with SDL3 from source on Debian/Ubuntu (SDL's own list, trimmed
# to what the app uses: X11, Wayland, audio, gamepads).
set -e
sudo=$(command -v sudo || true)
$sudo apt-get update
$sudo apt-get install -y --no-install-recommends build-essential cmake ninja-build pkg-config git python3 \
  libasound2-dev libpulse-dev libpipewire-0.3-dev \
  libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxtst-dev \
  libxkbcommon-dev libwayland-dev wayland-protocols libdecor-0-dev \
  libgl1-mesa-dev libgles2-mesa-dev libegl1-mesa-dev libdrm-dev libgbm-dev \
  libdbus-1-dev libibus-1.0-dev libudev-dev
