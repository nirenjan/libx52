# Automake for libusbx52 and associated utilities
#
# Copyright (C) 2012-2018 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0

ACLOCAL_AMFLAGS = -I m4

# libusb stub library for use by test programs
check_LTLIBRARIES = libusbx52.la

libusbx52_la_SOURCES = usb_x52_stub.c usb_x52_hotplug.c usb_x52_vector.c
libusbx52_la_CFLAGS = @LIBUSB_CFLAGS@ -pthread
libusbx52_la_LDFLAGS = -rpath /nowhere -module

# Utility programs for use by tests
check_PROGRAMS = x52test_create_device_list x52test_log_actions

x52test_create_device_list_SOURCES = util/create_device_list.c
x52test_create_device_list_CFLAGS = @LIBUSB_CFLAGS@ -pthread

x52test_log_actions_SOURCES = util/log_actions.c
x52test_log_actions_CFLAGS = @X52_INCLUDE@ @LIBUSB_CFLAGS@ -pthread
x52test_log_actions_LDADD = libusbx52.la

EXTRA_DIST = README.md libusbx52.h
