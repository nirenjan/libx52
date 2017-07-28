#!/bin/bash
# Script to run automated build and testing on Travis-CI

# Abort early in case of failure
set -e

# Generate the build files
./autogen.sh

# Create a temporary directory to store build artifacts
rm -rf build
mkdir build
cd build

# Run the configuration script
../configure

# Build and run any tests
make V=0
make check

# Verify that the distribution works
make distcheck

