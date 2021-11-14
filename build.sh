#!/usr/bin/env bash

set -eu

mkdir -p .build
cd .build

conan install ..
cmake ..
make

touchscreen on
i3-msg focus output eDP-1 >/dev/null

./bin/Isolinear

touchscreen off
i3-msg focus output DP-3 >/dev/null || true


