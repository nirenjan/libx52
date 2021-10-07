# Top level Automake for libx52
#
# Copyright (C) 2012-2018 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0

ACLOCAL_AMFLAGS = -I m4

# Build any support libraries first
SUBDIRS = lib

if USE_NLS
SUBDIRS += po
endif

#######################################################################
# Defaults
#######################################################################
bin_PROGRAMS =
check_PROGRAMS =
lib_LTLIBRARIES =
check_LTLIBRARIES =
pkgconfig_DATA =
TESTS =
EXTRA_DIST =
CLEANFILES =

x52includedir = $(includedir)/libx52
x52include_HEADERS =

LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) $(top_srcdir)/tap-driver.sh

include libx52/Makefile.am
include libx52util/Makefile.am
include libx52io/Makefile.am
include libusbx52/Makefile.am

include cli/Makefile.am
include joytest/Makefile.am
include evtest/Makefile.am
include daemon/Makefile.am
include udev/Makefile.am

include docs/Makefile.am

#######################################################################
# Doxygen support
#######################################################################
if HAVE_DOXYGEN
DXGEN = $(DXGEN_@AM_V@)
DXGEN_ = $(DXGEN_@AM_DEFAULT_V@)
DXGEN_0 = @printf "  DXGEN    $<\n";

SYSCONFDIR=@sysconfdir@
export SYSCONFDIR
docs/.stamp: Doxyfile
	$(DXGEN)$(DOXYGEN) $<
	$(AM_V_at)touch $@

all-local: docs/.stamp
clean-local:
	rm -rf $(top_builddir)/docs

man1_MANS = docs/man/man1/x52cli.1
$(man1_MANS): docs/.stamp

# Install Doxygen generated HTML documentation and manpages
install-data-local:
	$(INSTALL) -d $(DESTDIR)$(docdir)
	cp -R -P $(top_builddir)/docs/html $(DESTDIR)$(docdir)

uninstall-local:
	rm -rf $(DESTDIR)$(docdir)

endif

# Extra files that need to be in the distribution
EXTRA_DIST += \
	ABOUT-NLS \
	AUTHORS \
	ChangeLog.md \
	CONTRIBUTING.md \
	Doxyfile.in \
	DoxygenLayout.xml \
	INSTALL.md \
	LICENSE \
	README.md \
	config.rpath \
	configure.version \
	Version \
	gettext.h \
	usb-ids.h \
	po/README.md

