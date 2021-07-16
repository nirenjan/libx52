Installation instructions for x52pro-linux
==========================================

[![Build Status](https://www.travis-ci.org/nirenjan/x52pro-linux.svg?branch=master)](https://www.travis-ci.org/nirenjan/x52pro-linux)

Build has been tested on the following operating systems (x86-64 only):

* Ubuntu 16.04 LTS
* Ubuntu 18.04 LTS
* OS X 10.13.6

# Prerequisites

## Required Packages

* automake
* autoconf
* autopoint
* gettext
* hidapi
* [inih](https://github.com/benhoyt/inih)
* libtool
* libusb-1.0 + headers
* pkg-config
* python3 (3.6 or greater)

### Installation instructions

| Platform | Install instructions |
| -------- | -------------------- |
| Ubuntu   | `sudo apt-get install automake autoconf gettext autopoint libhidapi-dev libinih-dev libtool libusb-1.0-0-dev pkg-config python3` |
| MacOS + Homebrew  | `brew install automake autoconf gettext hidapi libtool libusb meson pkg-config python3` |
| Arch Linux | `pacman -S base-devel libusb hidapi libinih python` |

On MacOS, `inih` is not available as a Homebrew formula. You need to build and
install it manually using the following steps:

* Download and extract inih from Github
* From the inih source directory, run `meson build`, then run `meson install -C
  build`.

## Optional Packages

If you want to generate HTML documentation for the library, and manpages for
the utilities, you will need the following packages:

* doxygen
* rsync

You will also need the following packages to run the unit tests:

* faketime
* cmocka

# Installation Instructions

1. Clone the repository
2. Run autogen.sh
3. Run the following commands:
```
./configure --prefix=/usr
make && sudo make install
```

## Configuration options

### udev

The configuration system should automatically detect the udev rules directory,
but you can override it by using the following argument to `configure`:

```
--with-udevrulesdir=/path/to/udev/rules.d
```

### Input group

The udev rules that are installed provide read/write access to members of the
input devices group. This defaults to `plugdev`, but can be modified using
the following argument to `configure`:

```
--with-input-group=group
```
