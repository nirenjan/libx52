#!/bin/bash
# Generate Doxygen documentation
set -e

./autogen.sh
mkdir build
cd build
../configure --prefix=/usr --sysconfdir=/etc --localstatedir=/var
make docs/.stamp
