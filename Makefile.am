# Top level Automake for x52pro-linux
#
# Copyright (C) 2012-2018 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = lib utils tests

# Extra files that need to be in the distribution
EXTRA_DIST = AUTHORS ChangeLog.md README.md LICENSE Doxyfile.in

# Doxygen support
if HAVE_DOXYGEN
docs/.stamp:
	$(AM_V_GEN)$(DOXYGEN) Doxyfile
	$(AM_V_at)touch $@

CLEANFILES = docs/.stamp

all-local: docs/.stamp
clean-local:
	rm -rf $(top_builddir)/docs
endif
