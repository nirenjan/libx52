#!/bin/bash
# Run configure in the build directory and stop
# The generated files will be used to run Doxygen
set -e

./autogen.sh
mkdir build
cd build
../configure
