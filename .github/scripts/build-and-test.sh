#!/bin/bash
# Run the build and tests
set -e

meson setup -Dprefix=/usr -Dsysconfdir=/etc -Dlocalstatedir=/var -Dnls=enabled build
cd build
ninja
ninja test

# Print bugreport output
./x52bugreport

# Make sure that there are no changes to the source code
# This may happen if the source have changed with differences to the
# translation files and templates. Enabling this will allow us to catch
# missing/modified translations.
git diff --exit-code
