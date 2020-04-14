libx52 Hotplug Support
======================

This document describes a design for USB hotplug support in libx52. The idea is
to have a daemon process running at all times, which will update the hardware
when the X52/X52Pro is plugged in.

# Assumptions

The core assumption is that a maximum of 1 device is present at any time. There
is no goal to support more than 1 X52/X52Pro at the same time.

# LibUSB Hotplug Support

libusb-1.0 version 1.0.16 and newer support hotplug notifications when a device
is inserted or removed. These call a registered callback function which can read
the device descriptor, and take action accordingly.

# libx52 Hotplug design

The hotplug mechanism consists of the following functions:

```
int libx52_hotplug_init(libx52_hotplug_service **svc);
void libx52_hotplug_exit(libx52_hotplug_service *svc);

int libx52_hotplug_register_callback(libx52_hotplug_service *svc,
                                     libx52_hotplug_fn *callback_fn,
                                     void *user_data,
                                     libx52_hotplug_callback_handle **cb_handle);

int libx52_hotplug_deregister_callback(libx52_hotplug_callback_handle *cb_handle);

typedef void (*libx52_hotplug_fn)(bool inserted, void *user_data, libx52_device *dev);
```

The init function will take care of initializing libx52, so the user should
not call `libx52_init`. The exit function will also take care of deinitializing
libx52, so the user should not call `libx52_exit`.

The user may register any number of callbacks, and they are called in an
undefined order on device insertion and removal. Each register callback also
returns a callback handle in an output parameter. This callback handle can be
used to deregister the callback in the future, if necessary.

`libx52_hotplug_exit` will take care of deregistering all registered callbacks,
and will deinitialize libx52.

# Interaction with libusbx52

As of this writing, libusbx52 reads from a regular file, however, in order to
support hotplug, the read should read from a fifo. Under Linux, a program can
open a fifo for reading and writing, and this should allow the program to write
a small amount of data into the fifo and exit, and the program using libusbx52
can read from that file.

In this case, the data format should change to include an additional character
prior to the idVendor and idProduct fields. This additional character will
indicate whether the device is being inserted or removed.

libusbx52 must now spawn a separate thread to deal with the fifo. This thread
will read from the fifo, and callback the registered handler when an event
occurs.
