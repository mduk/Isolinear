#!/usr/bin/env bash

set -eu

mkdir -p .build
cd .build

conan install ..
cmake ..
make

touchscreen on
i3-msg focus output eDP-1 >/dev/null

case "$1" in
  timers) ./bin/Timers ;;
  *) ./bin/IsoMPD ;;
esac

touchscreen off
i3-msg focus output DP-3 >/dev/null || true


