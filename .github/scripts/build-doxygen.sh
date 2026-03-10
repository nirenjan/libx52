#!/bin/bash
# Generate Doxygen documentation
set -e

meson setup -Dprefix=/usr -Dsysconfdir=/etc -Dlocalstatedir=/var -Dnls=enabled build
cd build
ninja docs
