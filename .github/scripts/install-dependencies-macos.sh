#!/bin/bash
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
    inih \
    doxygen \
    rsync \
    cmocka

exit 0
