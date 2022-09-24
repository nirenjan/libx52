# Automake for libx52
#
# Copyright (C) 2012-2018 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0

lib_LTLIBRARIES += libx52.la

# Core libx52 library
# This library handles the USB communication between the host and the X52
# Libtool Version Info
# See: https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
libx52_v_CUR=6
libx52_v_AGE=4
libx52_v_REV=1
libx52_la_SOURCES = \
	libx52/x52_control.c \
	libx52/x52_core.c \
	libx52/x52_date_time.c \
	libx52/x52_mfd_led.c \
	libx52/x52_strerror.c \
	libx52/x52_stringify.c
libx52_la_CFLAGS = \
	@LIBUSB_CFLAGS@ \
	-DLOCALEDIR=\"$(localedir)\" \
	-I $(top_srcdir) \
	$(WARN_CFLAGS)
libx52_la_LDFLAGS = \
	-export-symbols-regex '^libx52_' \
	-version-info $(libx52_v_CUR):$(libx52_v_REV):$(libx52_v_AGE) @LIBUSB_LIBS@ \
	$(WARN_LDFLAGS)
libx52_la_LIBADD = @LTLIBINTL@

# Header files that need to be copied
x52include_HEADERS += libx52/libx52.h

# pkg-config files
pkgconfig_DATA += libx52/libx52.pc

if HAVE_CMOCKA
TESTS += libx52test libx52-string-test
check_PROGRAMS += libx52test libx52-string-test

nodist_libx52test_SOURCES = libx52/test_libx52.c
libx52test_SOURCES = $(libx52_la_SOURCES)
libx52test_CFLAGS = @LIBUSB_CFLAGS@ -DLOCALEDIR='"$(localedir)"' -I $(top_srcdir) -I $(top_srcdir)/libx52
libx52test_CFLAGS += -Dlibusb_control_transfer=__wrap_libusb_control_transfer
libx52test_LDFLAGS = @CMOCKA_LIBS@ @LIBUSB_LIBS@
libx52test_LDADD = libx52.la

CLEANFILES += libx52/test_libx52.c
test_libx52_c_DEPENDS = \
	$(srcdir)/libx52/x52_test_gen.py \
	$(srcdir)/libx52/x52_tests.json

libx52/test_libx52.c: $(test_libx52_c_DEPENDS)
	$(AM_V_GEN) $(PYTHON) $(test_libx52_c_DEPENDS) > $@

libx52_string_test_SOURCES = \
	libx52/test_strings.c \
	libx52/x52_stringify.c \
	libx52/x52_strerror.c
libx52_string_test_CFLAGS = -I $(top_srcdir) -I $(top_srcdir)/libx52
libx52_string_test_LDFLAGS = @CMOCKA_LIBS@
libx52_string_test_LDADD = libx52.la
endif

# Extra files that need to be in the distribution
EXTRA_DIST += \
	libx52/libx52.h libx52/x52_commands.h libx52/x52_common.h libx52/README.md

# Add test files to the distribution
EXTRA_DIST += libx52/x52_test_gen.py libx52/x52_tests.json
