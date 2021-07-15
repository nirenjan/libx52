# Automake for x52d
#
# Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = x52d

# Service daemon that manages the X52 device
x52d_SOURCES = \
	x52d_main.c x52d_config_parser.c x52d_config.c

x52d_CFLAGS = \
	-I $(top_srcdir) \
	-I $(top_srcdir)/lib/libx52io \
	-I $(top_srcdir)/lib/libx52 \
	-I $(top_srcdir)/lib/libx52util \
	-I $(top_srcdir)/lib/pinelog \
	-DSYSCONFDIR=\"$(sysconfdir)\" \
	-DLOCALEDIR=\"$(localedir)\" \
	-DLOGDIR=\"$(localstatedir)/log\" \
	-DRUNDIR=\"$(runstatedir)\" \
	@INIH_CFLAGS@ $(WARN_CFLAGS)

x52d_LDFLAGS = @INIH_LIBS@ $(WARN_LDFLAGS)
x52d_LDADD = ../lib/pinelog/libpinelog.la

EXTRA_DIST = \
	x52d_config.def \
	x52d_config.h \
	x52d_const.h \
	x52d.conf
