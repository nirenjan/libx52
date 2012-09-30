Saitek X52Pro joystick driver for Linux
=======================================

This project adds a new driver for the Saitek/MadCatz X52 Pro flight
control system. The X52 pro is a HOTAS (hand on throttle and stick)
with 7 axes, 39 buttons, 1 hat and 1 thumbstick and a multi-function
display which is programmable.

Currently, only Windows drivers are available from Saitek PLC, which
led me to develop a new Linux driver which can program the MFD and
the individual LEDs on the joystick. Although the standard usbhid
driver is capable of reading the joystick, it is not sufficient to
really utilize all the capabilities of this system.

This project is currently a work-in-progress. However a high level
outline of the current objectives are listed below:

* Write a kernel module and export sysfs interfaces to act as a
driver.
* Write a userspace program that can configure the kernel module
and create custom button mappings to keyboard or mouse events.
* Add interrupt handling and export a /dev/input/jsX interface.
* Allow userspace programs to register callbacks on MFD button events.
