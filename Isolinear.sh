#!/usr/bin/env bash

set -eu

i3-msg focus output eDP-1
touchscreen on
./Isolinear
touchscreen off
i3-msg focus output DP-3
