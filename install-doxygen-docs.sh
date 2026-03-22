#!/bin/sh
# Install Doxygen HTML and man trees from the build directory.
# Arguments are paths relative to the install prefix (MESON_INSTALL_DESTDIR_PREFIX).
set -e

doc_html="$1"
mandir="$2"

if [ -d "$MESON_BUILD_ROOT/docs/html" ]; then
  mkdir -p "$MESON_INSTALL_DESTDIR_PREFIX/$doc_html"
  cp -R "$MESON_BUILD_ROOT/docs/html"/. "$MESON_INSTALL_DESTDIR_PREFIX/$doc_html/"
fi

if [ -d "$MESON_BUILD_ROOT/docs/man" ]; then
  mkdir -p "$MESON_INSTALL_DESTDIR_PREFIX/$mandir"
  cp -R "$MESON_BUILD_ROOT/docs/man"/. "$MESON_INSTALL_DESTDIR_PREFIX/$mandir/"
fi
