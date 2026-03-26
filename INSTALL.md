Installation instructions for libx52
==========================================

Build has been tested on the following operating systems (x86-64 only):

* Ubuntu 22.04 LTS
* Ubuntu 24.04 LTS
* Fedora latest (Fedora 42, as of this commit)
* Archlinux
* Alpine Linux (Experimental)
* Ubuntu 26.04 LTS (Experimental as of this commit)
* macOS (latest tag on Github, ARM)

# Prerequisites

## Required Packages

* meson
* ninja
* gettext
* hidapi + headers
* inih
* libusb-1.0 + headers
* pkg-config
* python3 (3.6 or greater)
* git (not required for builds, but necessary to clone the repository)

### Installation instructions

| Platform | Install instructions |
| -------- | -------------------- |
| Ubuntu   | `sudo apt-get install meson gettext libhidapi-dev libevdev-dev libusb-1.0-0-dev libinih-dev pkg-config python3 git` |
| MacOS + Homebrew  | `brew install meson gettext hidapi libtool libusb pkg-config python3 git` |
| Arch Linux | `pacman -S base-devel meson libusb hidapi libevdev libinih python git` |
| Fedora | `sudo dnf install meson gettext-devel findutils hidapi-devel libusb-devel libevdev-devel inih-devel pkg-config python3 git` |

## Optional Packages

* doxygen - to generate HTML documentation and man pages
* libcmocka (1.1 or greater) + headers - to run unit tests
* libevdev + headers (on Linux) - to add virtual keyboard/mouse support

# Installation Instructions

1. Clone the repository
```
git clone https://github.com/nirenjan/libx52.git
```

2. Run autogen.sh
```
cd ./libx52
meson setup build -Dprefix=/usr
```

3. Run the following commands:
```
meson compile -C build && meson install -C build
```

You may want to remove or edit the `-Dprefix=/usr` option, most users prefer
non-distro binaries in `/usr/local` (default without `-Dprefix`) or `/opt`.

## Configuration options

### udev

The configuration system should automatically detect the udev rules directory,
but you can override it by using the following argument to `meson setup`:

```
-Dudev-rules-dir=/path/to/udev/rules.d
```

### Input group

The udev rules that are installed provide read/write access to members of the
input devices group. This defaults to `plugdev`, but can be modified using
the following argument to `meson setup`:

```
-Dinput-group=group
```

### Systemd support

The X52 daemon can run either as a foreground process, or it can daemonize
itself to run in the background. Typical deployments with systemd will have it
run in the foreground, and disable timestamps in the logs, since those are
inserted automatically by journald.

Systemd support is enabled by default, which disables timestamps in the program
logs, but you can re-enable timestamps by passing `-Dsystemd-logs=disabled`
argument to `meson setup`

It is also possible to configure the directory in which the service file is
installed with the following option. This is ignored if systemd is not found.

```
-Dsystemd-unit-dir=/path/to/systemd/system
```
