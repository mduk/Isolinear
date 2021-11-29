#!/usr/bin/bash

set -eu

cd .build

touchscreen on
i3-msg focus output eDP-1 >/dev/null

gdb -ex run ./bin/IsoMPD

touchscreen off
i3-msg focus output DP-3 >/dev/null || true

