# Automake for libx52-bugreport
#
# Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0

bin_PROGRAMS += x52bugreport

# Bug report program that reports the build and linked library versions
x52bugreport_SOURCES = bugreport/bugreport.c
x52bugreport_CFLAGS = \
	-I$(top_srcdir)/libx52io \
	@LIBUSB_CFLAGS@ \
	@HIDAPI_CFLAGS@ \
	$(WARN_CFLAGS)

x52bugreport_LDFLAGS = \
	@LIBUSB_LIBS@ \
	@HIDAPI_LIBS@ \
	$(WARN_LDFLAGS)

x52bugreport_LDADD = libx52io.la
