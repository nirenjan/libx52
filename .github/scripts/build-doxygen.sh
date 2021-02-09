#!/bin/bash
# Generate Doxygen documentation
set -e

./autogen.sh
mkdir build
cd build
../configure
make docs/.stamp
