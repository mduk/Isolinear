#!/usr/bin/env bash

set -eu

i3-msg focus output eDP-1
touchscreen on
./.build/bin/IsoMPD
touchscreen off
i3-msg focus output DP-3
