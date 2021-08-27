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

# Make sure that there are no changes to the source code
# This may happen if the source have changed with differences to the
# translation files and templates. Enabling this will allow us to catch
# missing/modified translations.
git diff --exit-code
