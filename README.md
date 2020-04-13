Saitek X52Pro joystick driver for Linux
=======================================

This project adds a new driver for the Saitek/MadCatz X52 Pro flight
control system. The X52 pro is a HOTAS (hand on throttle and stick)
with 7 axes, 39 buttons, 1 hat and 1 thumbstick and a multi-function
display which is programmable.

Currently, only Windows drivers are available from Saitek PLC, which
led me to develop a new Linux driver which can program the MFD and
the individual LEDs on the joystick. The standard usbhid driver is
capable of reading the joystick, but it cannot control the MFD or LEDs.

Most of the extra functionality can be handled from userspace. See
the individual folders for README information.

# Building

[![Build Status](https://www.travis-ci.org/nirenjan/x52pro-linux.svg?branch=master)](https://www.travis-ci.org/nirenjan/x52pro-linux)

Build has been tested on the following operating systems (x86-64 only):

* Ubuntu 14.04 LTS
* Ubuntu 16.04 LTS
* Ubuntu 18.04 LTS
* OS X 10.13.6

## Prerequisites

You will need the following packages:

* automake
* autoconf
* libtool
* libusb-1.0-0-dev (Package name may vary across distributions)
* pkg-config
* python (2.6 or greater)

If you are on OSX, you can install the above packages using Homebrew

## Installation

1. Clone the repository
2. Run autogen.sh
3. Run configure; make and sudo make install.

