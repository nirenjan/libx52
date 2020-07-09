# Automake for libx52io
#
# Copyright (C) 2012-2020 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0

lib_LTLIBRARIES = libx52io.la

# X52 IO library
# This library handles the HID parsing of the X52 USB reports
# Libtool Version Info
# See: https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
libx52io_v_CUR=0
libx52io_v_AGE=0
libx52io_v_REV=0
libx52io_la_SOURCES = io_core.c io_axis.c io_parser.c
libx52io_la_CFLAGS = @HIDAPI_CFLAGS@ -DLOCALEDIR=\"$(localedir)\" -I $(top_srcdir) $(WARN_CFLAGS)
libx52io_la_LDFLAGS = \
	-export-symbols-regex '^libx52io_' \
	-version-info $(libx52io_v_CUR):$(libx52io_v_REV):$(libx52io_v_AGE) @HIDAPI_LIBS@ \
	$(WARN_LDFLAGS)
libx52io_la_LIBADD = @LTLIBINTL@

# Header files that need to be copied
x52includedir = $(includedir)/x52pro
x52include_HEADERS = libx52io.h

# pkg-config files
# pkgconfig_DATA = libx52io.pc

# Extra files that need to be in the distribution
EXTRA_DIST = libx52io.h io_common.h
