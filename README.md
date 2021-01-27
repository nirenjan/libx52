Saitek X52Pro joystick driver for Linux
=======================================

![Build/Test](https://github.com/nirenjan/x52pro-linux/workflows/Build/Test/badge.svg)
![Kernel Module](https://github.com/nirenjan/x52pro-linux/workflows/Kernel%20Module/badge.svg)
![CodeQL](https://github.com/nirenjan/x52pro-linux/workflows/CodeQL/badge.svg)

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

**Note:** This repository currently only provides commandline interfaces to
control the MFD and LEDs. If you are not comfortable working in the commandline,
then the [gx52](https://gitlab.com/leinardi/gx52) project might be a better fit
for your needs as it provides a graphical interface to control the MFD and LEDs.

# Building and installing

See [INSTALL.md](https://github.com/nirenjan/x52pro-linux/blob/master/INSTALL.md)
