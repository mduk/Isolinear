#!/usr/bin/env bash

set -eu

declare proj="${1:-}"

mkdir -p .build
cd .build

conan install ..
cmake ..
make $proj

if [[ ! -z $proj ]]
then
  touchscreen on
  i3-msg focus output eDP-1 >/dev/null

  ./bin/$proj

  touchscreen off
  i3-msg focus output DP-3 >/dev/null || true
fi
