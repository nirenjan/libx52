#!/bin/bash
# Install dependencies to build and test on Ubuntu runners
sudo apt-get update
sudo apt-get install -y \
    autoconf \
    automake \
    libtool \
    pkg-config \
    python3 \
    gettext \
    autopoint \
    libusb-1.0-0-dev \
    libhidapi-dev \
    libevdev-dev \
    doxygen \
    libcmocka-dev \
    faketime \
    meson

exit 0
