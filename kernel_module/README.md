Kernel driver for Saitek X52 and X52 Pro
========================================

This folder contains a loadable kernel module for the Saitek X52 and X52 Pro
HOTAS. This improves upon the standard `hid-generic` driver by reporting both
the left-right and up-down motion of the thumb stick.

However, it changes the buttons that are reported by the joystick, and thus,
may not be suitable for all applications.

**Note:** The following kernel versions have a fix to handle the thumb stick,
and therefore, do not require this driver.

* 5.9+ (from -rc4 onwards)
* 5.8.10+
* 5.4.66+
* 4.19.146+

# Building

This directory is deliberately not integrated with the top level Autotools
based build framework.

Install the Linux headers for the currently running kernel. On Ubuntu, this
can be done by running `sudo apt-get install -y linux-headers-$(uname -r)`.

Run `make`. This will build the module from source.

# Installing the kernel module

Once you have built the kernel module, run `sudo insmod saitek_x52.ko` from
the current directory. With a recent enough kernel, the driver should switch
automatically. Otherwise, simply disconnect and reconnect your X52.

# Reporting issues

Please report any issues seen as a [Github issue](https://github.com/nirenjan/libx52/issues).
