# Top level Automake for x52pro-linux
#
# Copyright (C) 2012-2018 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0

ACLOCAL_AMFLAGS = -I m4

if USE_NLS
po_SUBDIRS = po
endif

SUBDIRS = $(po_SUBDIRS) lib utils tests udev

# Extra files that need to be in the distribution
EXTRA_DIST = \
	ABOUT-NLS \
	AUTHORS \
	ChangeLog.md \
	Doxyfile.in \
	INSTALL.md \
	LICENSE \
	README.md \
	config.rpath \
	gettext.h \
	po/README.md

# Doxygen support
if HAVE_DOXYGEN
DXGEN = $(DXGEN_@AM_V@)
DXGEN_ = $(DXGEN_@AM_DEFAULT_V@)
DXGEN_0 = @printf "  DXGEN    $<\n";
docs/.stamp: Doxyfile
	$(DXGEN)$(DOXYGEN) $<
	$(AM_V_at)touch $@

all-local: docs/.stamp
clean-local:
	rm -rf $(top_builddir)/docs

# Install Doxygen generated HTML documentation and manpages
install-data-local:
	$(INSTALL) -d $(DESTDIR)$(docdir)
	rsync -a $(top_builddir)/docs/html $(DESTDIR)$(docdir)

	$(INSTALL) -d $(DESTDIR)$(mandir)/man1
	$(INSTALL) -D docs/man/man1/x52cli.1 $(DESTDIR)$(mandir)/man1

uninstall-local:
	rm -rf $(DESTDIR)$(docdir)
	rm -rf $(DESTDIR)$(mandir)/man1/x52cli.1

endif
