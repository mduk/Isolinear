#!/usr/bin/env bash

set -eu

declare proj="${1:-IsoMPD}"

mkdir -p .build
cd .build

conan install ..
cmake ..
make $proj

touchscreen on
i3-msg focus output eDP-1 >/dev/null

./bin/$proj

touchscreen off
i3-msg focus output DP-3 >/dev/null || true


