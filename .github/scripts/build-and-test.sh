#!/bin/bash
# Run the build and tests
set -e

BUILDDIR="${1:-build}"

rm -rf "$BUILDDIR"

# Handle the meson dist failure in CI
if [[ "$GITHUB_ACTIONS" == "true" ]]
then
    # If in a container, then use the system directory
    git config --system --add safe.directory '*' || \
        git config --global --add safe.directory '*'
fi

meson setup -Dprefix=/usr -Dsysconfdir=/etc -Dlocalstatedir=/var -Dnls=enabled "$BUILDDIR"
cd "$BUILDDIR"
meson compile
meson test

if [[ $(printf "%s\n" "0.62.0" "$(meson --version)" | sort -V | head -1) == "0.62.0" ]]
then
    meson dist --allow-dirty # Required to fix CI build
else
    meson dist
fi

# Print bugreport output
./bugreport/x52bugreport

# Make sure that there are no changes to the source code
# This may happen if the source have changed with differences to the
# translation files and templates. Enabling this will allow us to catch
# missing/modified translations.
git diff --exit-code
