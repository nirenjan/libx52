# Top level Automake for x52pro-linux
#
# Copyright (C) 2012-2018 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0

ACLOCAL_AMFLAGS = -I m4

if USE_NLS
po_SUBDIRS = po
endif

SUBDIRS = $(po_SUBDIRS) lib utils tests

# Extra files that need to be in the distribution
EXTRA_DIST = \
	ABOUT-NLS \
	AUTHORS \
	ChangeLog.md \
	Doxyfile.in \
	LICENSE \
	README.md \
	config.rpath \
	gettext.h \
	po/README.md

# Doxygen support
if HAVE_DOXYGEN
docs/.stamp: Doxyfile
	$(AM_V_GEN)$(DOXYGEN) $<
	$(AM_V_at)touch $@

CLEANFILES = docs/.stamp

all-local: docs/.stamp
clean-local:
	rm -rf $(top_builddir)/docs
endif
