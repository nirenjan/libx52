#!/bin/sh
# Install Doxygen HTML and man trees from the build directory.
# Arguments are paths relative to the install prefix (MESON_INSTALL_DESTDIR_PREFIX).
set -e

doc_html="$1"
mandir="$2"

# Installed manual pages from the Doxygen man tree (see GENERATE_MAN in Doxyfile.in).
WANTED_PAGES="man1/x52cli.1 man1/x52bugreport.1 man1/x52d.1 man1/x52d_protocol.1 man1/x52dcomm.1 man1/proto_lipc_framed.1 man1/x52ctl.1"

if [ -d "$MESON_BUILD_ROOT/docs/html" ]; then
  mkdir -p "$MESON_INSTALL_DESTDIR_PREFIX/$doc_html"
  cp -R "$MESON_BUILD_ROOT/docs/html"/. "$MESON_INSTALL_DESTDIR_PREFIX/$doc_html/"
fi

if [ -d "$MESON_BUILD_ROOT/docs/man" ]; then
  MANDIR="$MESON_INSTALL_DESTDIR_PREFIX/$mandir"
  mkdir -p "$MANDIR"
  for manpage in $WANTED_PAGES
  do
    section=$(dirname "$manpage")
    mkdir -p "$MANDIR/$section"
    cp "$MESON_BUILD_ROOT/docs/man/$manpage" "$MANDIR/$section"
  done
fi
