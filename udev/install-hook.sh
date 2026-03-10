#!/bin/sh -x
# Run the following command when installing files

# If DESTDIR is not specified or is not /, quit early
if ! test -n "${DESTDIR}" || [ "${DESTDIR}" != "/" ]
then
    exit 0
fi

if [ "$(id -u)" = "0" ]
then
    udevadm control --reload-rules && \
    udevadm trigger --subsystem-match=usb --attr-match=idVendor=06a3 --action=add
fi

true
