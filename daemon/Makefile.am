# Automake for x52d
#
# Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
bin_PROGRAMS += x52d

# Service daemon that manages the X52 device
x52d_SOURCES = \
	daemon/x52d_main.c \
	daemon/x52d_config_parser.c \
	daemon/x52d_config.c \
	daemon/x52d_device.c \
	daemon/x52d_clock.c \
	daemon/x52d_led.c

x52d_CFLAGS = \
	-I $(top_srcdir) \
	-I $(top_srcdir)/libx52io \
	-I $(top_srcdir)/libx52 \
	-I $(top_srcdir)/libx52util \
	-I $(top_srcdir)/lib/pinelog \
	-DSYSCONFDIR=\"$(sysconfdir)\" \
	-DLOCALEDIR=\"$(localedir)\" \
	-DLOGDIR=\"$(localstatedir)/log\" \
	-DRUNDIR=\"$(runstatedir)\" \
	@INIH_CFLAGS@ @PTHREAD_CFLAGS@ $(WARN_CFLAGS)

x52d_LDFLAGS = @INIH_LIBS@ @PTHREAD_LIBS@ $(WARN_LDFLAGS)
x52d_LDADD = \
	lib/pinelog/libpinelog.la \
	libx52.la \
	@LTLIBINTL@

x52dconfdir = @sysconfdir@/x52d
x52dconf_DATA = daemon/x52d.conf

EXTRA_DIST += \
	daemon/x52d_clock.h \
	daemon/x52d_config.def \
	daemon/x52d_config.h \
	daemon/x52d_const.h \
	daemon/x52d_device.h \
	daemon/x52d.conf