# Automake for libx52
#
# Copyright (C) 2012-2018 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0

ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libx52.la

# Core libx52 library
# This library handles the USB communication between the host and the X52
# Libtool Version Info
# See: https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
libx52_v_CUR=4
libx52_v_AGE=2
libx52_v_REV=1
libx52_la_SOURCES = x52_control.c x52_core.c x52_date_time.c x52_mfd_led.c \
					x52_strerror.c
libx52_la_CFLAGS = @LIBUSB_CFLAGS@ -DLOCALEDIR=\"$(localedir)\" -I $(top_srcdir)
libx52_la_LDFLAGS = \
	-export-symbols-regex '^libx52_' \
	-version-info $(libx52_v_CUR):$(libx52_v_REV):$(libx52_v_AGE) @LIBUSB_LIBS@
libx52_la_LIBADD = @LTLIBINTL@

# Header files that need to be copied
x52includedir = $(includedir)/x52pro
x52include_HEADERS = libx52.h

# pkg-config files
pkgconfig_DATA = libx52.pc

check_PROGRAMS = test_offset test_led test_blink_shift

test_offset_SOURCES = test_offset.c
test_offset_CFLAGS = @LIBUSB_CFLAGS@
test_offset_LDADD = libx52.la

test_led_SOURCES = test_led.c test_common.c
test_led_CFLAGS = @LIBUSB_CFLAGS@
test_led_LDADD = libx52.la

test_blink_shift_SOURCES = test_blink_shift.c test_common.c
test_blink_shift_CFLAGS = @LIBUSB_CFLAGS@
test_blink_shift_LDADD = libx52.la

LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) $(top_srcdir)/tap-driver.sh
TESTS = test_offset test_led test_blink_shift

# Extra files that need to be in the distribution
EXTRA_DIST = libx52.h x52_commands.h x52_common.h test_common.h README.md

# Add documentation files to the distribution
EXTRA_DIST += \
	doc/main.dox \
	doc/caveats.dox \
	doc/integration.dox
