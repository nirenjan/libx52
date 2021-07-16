#!/bin/bash -x
# Install dependencies to build and test on Ubuntu runners
brew install \
    autoconf \
    automake \
    libtool \
    pkg-config \
    python3 \
    gettext \
    libusb \
    hidapi \
    doxygen \
    rsync \
    meson \
    cmocka

# inih cannot be installed via Homebrew, install it manually
INIH_VER=r53
curl -LO https://github.com/benhoyt/inih/archive/refs/tags/${INIH_VER}.tar.gz
tar xvf ${INIH_VER}.tar.gz
cd inih-${INIH_VER}
meson build
meson install -C build

exit 0
