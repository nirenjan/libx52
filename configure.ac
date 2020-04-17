# Autoconf settings for x52pro-linux
#
# Copyright (C) 2012-2018 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0

AC_INIT([x52pro-linux], [0.2.1], [nirenjan@gmail.com])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_PROG_CC
AC_PROG_CC_STDC
AM_PROG_AR
AM_PATH_PYTHON([2.6])
LT_INIT
PKG_PROG_PKG_CONFIG
PKG_INSTALLDIR

# Check for libusb-1.0
AX_PKG_CHECK_MODULES([LIBUSB], [libusb-1.0])

AC_SUBST([LIBUSB_CFLAGS])
AC_SUBST([LIBUSB_LDFLAGS])
AC_SUBST([LIBUSB_LIBS])

AC_SUBST([X52_PKG_VERSION], [0.1])
AC_SUBST([X52_INCLUDE], ["-I \$(top_srcdir)/lib/libx52"])

# Check for pthreads
ACX_PTHREAD
# LIBS="$PTHREAD_LIBS $LIBS"
# CFLAGS="$PTHREAD_CFLAGS $CFLAGS"
# CC="$PTHREAD_CC"

AC_CONFIG_FILES([
    Makefile
    lib/Makefile
    lib/libx52/Makefile
    lib/libx52/libx52.pc
    lib/libusbx52/Makefile
    lib/libx52util/Makefile
    utils/Makefile
    utils/cli/Makefile
    utils/test/Makefile
    tests/Makefile
])
AC_OUTPUT
