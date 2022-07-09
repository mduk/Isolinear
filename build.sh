#!/usr/bin/env bash

set -eu

sudo apt install -y libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libsdl2-gfx-dev \
  libx11-xcb-dev libxaw7-dev libxcomposite-dev libxdamage-dev libxft-dev libxkbfile-dev \
  libxmuu-dev libxres-dev libxtst-dev libxvmc-dev libxcb-* \
  libasio-dev libfmt-dev \
  cmake

declare proj="${1:-}"

mkdir -p build

cmake -B./build -S.

if [[ ! -z $proj ]]
then
  ./build/$proj
fi
