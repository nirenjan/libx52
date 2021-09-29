Installation instructions for libx52
==========================================

Build has been tested on the following operating systems (x86-64 only):

* Ubuntu 18.04 LTS
* Ubuntu 20.04 LTS
* OS X 10.13.6

# Prerequisites

## Required Packages

* automake
* autoconf
* autopoint
* gettext
* hidapi + headers
* libtool
* libusb-1.0 + headers
* libevdev + headers (on Linux)
* pkg-config
* python3 (3.6 or greater)
* git (not required for builds, but necessary to clone the repository)

### Installation instructions

| Platform | Install instructions |
| -------- | -------------------- |
| Ubuntu   | `sudo apt-get install automake autoconf gettext autopoint libhidapi-dev libevdev-dev libtool libusb-1.0-0-dev pkg-config python3 git` |
| MacOS + Homebrew  | `brew install automake autoconf gettext hidapi libtool libusb pkg-config python3 git` |
| Arch Linux | `pacman -S base-devel libusb hidapi libevdev python git` |
| Fedora | `sudo dnf install autoconf automake gettext-devel findutils libtool hidapi-devel libusb-devel libevdev-devel pkg-config python3 git` |

## Optional Packages

* doxygen - to generate HTML documentation and man pages
* libcmocka (1.1 or greater) + headers - to run unit tests

# Installation Instructions

1. Clone the repository
```
git clone https://github.com/nirenjan/libx52.git
```

2. Run autogen.sh
```
cd ./libx52
./autogen.sh
```

3. Run the following commands:
```
./configure --prefix=/usr --localstatedir=/var --sysconfdir=/etc
make && sudo make install
```

You may want to remove or edit the `--prefix=/usr` option, most users prefer
non-distro binaries in `/usr/local` (default without `--prefix`) or `/opt`.

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

### Systemd support

The X52 daemon can run either as a foreground process, or it can daemonize
itself to run in the background. Typical deployments with systemd will have it
run in the foreground, and disable timestamps in the logs, since those are
inserted automatically by journald.

Systemd support is enabled by default, but can be disabled with the
`--disable-systemd` argument to `configure`

It is also possible to configure the directory in which the service file is
installed with the following option. This is ignored if you have specified
`--disable-systemd`.

```
--with-systemdsystemunitdir=/path/to/systemd/system
```
