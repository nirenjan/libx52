# Automake for libx52
#
# Copyright (C) 2012-2018 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0

ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libx52.la

# Core libx52 library
# This library handles the USB communication between the host and the X52
# Libtool Version Info
# See: https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
libx52_v_CUR=4
libx52_v_AGE=2
libx52_v_REV=1
libx52_la_SOURCES = x52_control.c x52_core.c x52_date_time.c x52_mfd_led.c \
					x52_strerror.c
libx52_la_CFLAGS = @LIBUSB_CFLAGS@ -DLOCALEDIR=\"$(localedir)\" -I $(top_srcdir) $(WARN_CFLAGS)
libx52_la_LDFLAGS = \
	-export-symbols-regex '^libx52_' \
	-version-info $(libx52_v_CUR):$(libx52_v_REV):$(libx52_v_AGE) @LIBUSB_LIBS@ \
	$(WARN_LDFLAGS)
libx52_la_LIBADD = @LTLIBINTL@

# Header files that need to be copied
x52includedir = $(includedir)/x52pro
x52include_HEADERS = libx52.h

# pkg-config files
pkgconfig_DATA = libx52.pc

if HAVE_CMOCKA
LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) $(top_srcdir)/tap-driver.sh
TESTS = libx52test
check_PROGRAMS = libx52test

libx52test_SOURCES = test_libx52.c $(libx52_la_SOURCES)
libx52test_CFLAGS = @LIBUSB_CFLAGS@ -DLOCALEDIR='"$(localedir)"' -I $(top_srcdir)
libx52test_CFLAGS += -DGENERATED_TESTS='"test_libx52.c"'
libx52test_LDFLAGS = -Wl,--wrap=libusb_control_transfer @CMOCKA_LIBS@ @LIBUSB_LIBS@
libx52test_LDADD = libx52.la

CLEANFILES = test_libx52.c
test_libx52.c: $(srcdir)/x52_test_gen.py $(srcdir)/x52_tests.json
	$(AM_V_GEN) $(PYTHON) $(srcdir)/x52_test_gen.py $(srcdir)/x52_tests.json > $@
endif

# Extra files that need to be in the distribution
EXTRA_DIST = libx52.h x52_commands.h x52_common.h README.md

# Add test files to the distribution
EXTRA_DIST += x52_test_gen.py x52_tests.json

# Add documentation files to the distribution
EXTRA_DIST += \
	doc/main.dox \
	doc/caveats.dox \
	doc/integration.dox
