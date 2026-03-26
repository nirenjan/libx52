#!/bin/bash
# Install dependencies on Fedora container
set -euo pipefail

dnf update -y
dnf install -y \
    gcc \
    git \
    meson \
    libusb1-devel \
    hidapi-devel \
    inih-devel \
    libevdev-devel \
    pkg-config \
    python3 \
    gettext-devel
