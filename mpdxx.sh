#!/usr/bin/env bash

set -eu

mkdir -p .build
cd .build

conan install ..
cmake ..
make

./bin/MPDXX


