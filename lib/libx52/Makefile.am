# Automake for libx52
#
# Copyright (C) 2012-2018 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0

ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libx52.la

# Core libx52 library
# This library handles the USB communication between the host and the X52
libx52_la_SOURCES = x52_control.c x52_core.c x52_date_time.c x52_mfd_led.c \
					x52_strerror.c
libx52_la_CFLAGS = @LIBUSB_CFLAGS@ -DLOCALEDIR=\"$(localedir)\" -I $(top_srcdir)
libx52_la_LDFLAGS = -version-info 4:0:2 @LIBUSB_LIBS@
libx52_la_LIBADD = @LTLIBINTL@

# Header files that need to be copied
x52includedir = $(includedir)/x52pro
x52include_HEADERS = libx52.h

# pkg-config files
pkgconfig_DATA = libx52.pc

# Extra files that need to be in the distribution
EXTRA_DIST = libx52.h x52_commands.h x52_common.h README.md

# Add documentation files to the distribution
EXTRA_DIST += \
	doc/main.dox \
	doc/api.dox \
	doc/caveats.dox \
	doc/integration.dox
