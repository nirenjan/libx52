#!/bin/bash
# Run the build and tests
set -e

meson setup build
cd build
meson compile
meson test

# Print bugreport output
./x52bugreport

# Make sure that there are no changes to the source code
# This may happen if the source have changed with differences to the
# translation files and templates. Enabling this will allow us to catch
# missing/modified translations.
git diff --exit-code
