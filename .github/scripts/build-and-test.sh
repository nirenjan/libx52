#!/bin/bash
# Run the build and tests
set -e

BUILDDIR="${1:-build}"

rm -rf "$BUILDDIR"

# Handle the meson dist failure in CI
if [[ "$GITHUB_ACTIONS" == "true" ]]
then
    git config --global --add safe.directory '*'
fi

meson setup -Dprefix=/usr -Dsysconfdir=/etc -Dlocalstatedir=/var -Dnls=enabled "$BUILDDIR"
cd "$BUILDDIR"
meson compile
meson test
meson dist

# Print bugreport output
./bugreport/x52bugreport

# Make sure that there are no changes to the source code
# This may happen if the source have changed with differences to the
# translation files and templates. Enabling this will allow us to catch
# missing/modified translations.
git diff --exit-code
