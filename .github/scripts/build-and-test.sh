#!/bin/bash
# Run the build and tests
set -e

./autogen.sh
mkdir build
cd build
../configure
make -j V=0
make -j check V=0
make -j distcheck
