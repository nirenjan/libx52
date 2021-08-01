# Automake for libusbx52 and associated utilities
#
# Copyright (C) 2012-2018 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0

# libusb stub library for use by test programs
check_LTLIBRARIES += libusbx52.la

libusbx52_la_SOURCES = libusbx52/usb_x52_stub.c libusbx52/fopen_env.c
libusbx52_la_CFLAGS = -I $(top_srcdir)/libusbx52 @LIBUSB_CFLAGS@ $(WARN_CFLAGS)
libusbx52_la_LDFLAGS = -rpath /nowhere -module $(WARN_LDFLAGS)

# Utility programs for use by tests
check_PROGRAMS += x52test_create_device_list x52test_log_actions

x52test_create_device_list_SOURCES = libusbx52/util/create_device_list.c $(libusbx52_la_SOURCES)
x52test_create_device_list_CFLAGS = $(libusbx52_la_CFLAGS)
x52test_create_device_list_LDFLAGS = $(WARN_LDFLAGS)

x52test_log_actions_SOURCES = libusbx52/util/log_actions.c $(libusbx52_la_SOURCES)
x52test_log_actions_CFLAGS = -I $(top_srcdir)/libx52 $(libusbx52_la_CFLAGS)
x52test_log_actions_LDFLAGS = $(WARN_LDFLAGS)

EXTRA_DIST += libusbx52/README.md libusbx52/libusbx52.h
