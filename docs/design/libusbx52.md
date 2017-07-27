LibUSB X52 stub library
=======================

The libusbx52 stub library is a convenience library to help test the libusb
functions used by libx52 and associated code. It simulates the behavior of the
libusb functions used by the libx52 library, but doesn't actually control any
real hardware, and simply updates a few in-memory data structures.

The idea behind `libusbx52.so` is to use it as an LD_PRELOAD library, where it
will override the real functions used by `libx52.so`. The use case for this
scenario is in an automated testing environment, where a test runner could set
up the list of devices manually and simulate various scenarios.

# Design Overview

Unfortunately, the automake infrastructure does not support the use of
LD_PRELOAD because it is deemed "non-portable" in the automake sense. As a
result, this is now up to a test runner application to implement a method to
control the data passed between two processes.

# Data Structures

The server process is responsible for setting up the initial set of USB devices.
As far as libx52 is concerned, the only fields that it uses in the USB
descriptor are the idVendor and idProduct fields. These are written to a file
that is read by the libusbx52 stubs to populate the device list.

Once the file has been written by the server, the libusb_init stub function in
the client can read the file and populate the internal data structures as
necessary.

The client can also write to a separate file to record the USB communication
sent across to the simulated device.


