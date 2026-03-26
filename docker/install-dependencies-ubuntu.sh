#!/bin/bash
# Install dependencies to build and test on Ubuntu runners
apt-get update && apt-get upgrade -y
apt-get install -y \
    git \
    gcc clang \
    meson \
    gettext \
    pkg-config \
    python3 \
    libusb-1.0-0-dev \
    libhidapi-dev \
    libevdev-dev \
    libinih-dev \
    doxygen \
    libcmocka-dev \
    tzdata

exit 0
