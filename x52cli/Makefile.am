# Automake for x52cli
#
# Copyright (C) 2012-2018 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = x52cli

# Command line utility that front ends the core library
x52cli_SOURCES = x52_cli.c
x52cli_CFLAGS = -I $(top_srcdir)/libx52 $(WARN_CFLAGS)
x52cli_LDFLAGS = $(WARN_LDFLAGS)
x52cli_LDADD = ../libx52/libx52.la
