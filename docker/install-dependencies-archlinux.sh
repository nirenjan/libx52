#!/bin/bash
# Install dependencies on Archlinux container
# Assumes that it's running off a base-devel tag
set -euo pipefail

pacman -Syu --noconfirm \
    git \
    meson \
    libusb \
    hidapi \
    libinih \
    libevdev \
    python \
    gettext
