# Automake for x52evtest
#
# Copyright (C) 2012-2020 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = x52evtest

# Event test utility that works similarly to the Linux evtest
x52evtest_SOURCES = ev_test.c
x52evtest_CFLAGS = -I $(top_srcdir)/lib/libx52io -I $(top_srcdir) -DLOCALEDIR=\"$(localedir)\" $(WARN_CFLAGS)
x52evtest_LDFLAGS = $(WARN_LDFLAGS)
x52evtest_LDADD = ../../lib/libx52io/libx52io.la
