#!/bin/sh
# Setup for alpine
set -eux

apk update
apk add --no-cache \
  build-base \
  meson \
  bash \
  git \
  gettext \
  libusb-dev \
  hidapi-dev \
  libevdev-dev \
  inih-dev \
  cmocka-dev \
  tzdata \
  musl-libintl \
  doxygen
