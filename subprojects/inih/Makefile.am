# Top level Automake for inih
#
# Makefile copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# inih is copyright (C) 2009-2020, Ben Hoyt
# inih is released under the New BSD license (see LICENSE.txt). Go to the
# project home page for more info:
# https://github.com/benhoyt/inih

ACLOCAL_AMFLAGS = -I m4

# Extra files that need to be in the distribution
EXTRA_DIST = \
	LICENSE.txt \
	ini.h \
	README.md

noinst_LTLIBRARIES = libinih.la

libinih_la_SOURCES = ini.c
libinih_la_CFLAGS = $(WARN_CFLAGS) -I $(top_builddir)
libinih_la_LDFLAGS = $(WARN_LDFLAGS)
