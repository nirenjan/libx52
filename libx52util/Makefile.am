# Automake for libx52util
#
# Copyright (C) 2012-2018 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0

lib_LTLIBRARIES += libx52util.la

# libx52 utility library
# This library provides extra utilities for ease of use
nodist_libx52util_la_SOURCES = libx52util/util_char_map.c
libx52util_la_SOURCES = libx52util/x52_char_map_lookup.c
libx52util_la_CFLAGS = -I $(top_srcdir)/libx52util $(WARN_CFLAGS)
libx52util_la_LDFLAGS = -version-info 1:0:0 $(WARN_LDFLAGS)

# Header files that need to be copied
x52include_HEADERS += libx52util/libx52util.h

# pkg-config files
pkgconfig_DATA += libx52util/libx52util.pc

# Autogenerated file that needs to be cleaned up
CLEANFILES += libx52util/util_char_map.c
util_char_map_c_DEPENDS = \
	$(srcdir)/libx52util/x52_char_map_gen.py \
	$(srcdir)/libx52util/x52_char_map.cfg

libx52util/util_char_map.c: $(util_char_map_c_DEPENDS)
	$(AM_V_GEN) $(PYTHON) $(util_char_map_c_DEPENDS) $@

# Extra files that need to be in the distribution
EXTRA_DIST += 	libx52util/x52_char_map.cfg \
				libx52util/x52_char_map.h \
				libx52util/x52_char_map_gen.py

