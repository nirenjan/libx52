# Autoconf settings for x52pro-linux
#
# Copyright (C) 2012-2018 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0

AC_INIT([x52pro-linux], [0.2.1], [nirenjan@gmail.com])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall foreign subdir-objects])
AC_PROG_CC
AC_PROG_CC_STDC
AM_PROG_AR
AM_PATH_PYTHON([2.6])
LT_INIT
PKG_PROG_PKG_CONFIG
PKG_INSTALLDIR

# Internationalization
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION(0.18)
AM_CONDITIONAL([USE_NLS], [test "x${USE_NLS}" == "xyes"])

# Check for libusb-1.0
AX_PKG_CHECK_MODULES([LIBUSB], [libusb-1.0])

AC_SUBST([LIBUSB_CFLAGS])
AC_SUBST([LIBUSB_LDFLAGS])
AC_SUBST([LIBUSB_LIBS])

AC_SUBST([X52_PKG_VERSION], [0.1])
AC_SUBST([X52_INCLUDE], ["-I \$(top_srcdir)/lib/libx52"])

# Check for pthreads
ACX_PTHREAD

# make distcheck doesn't work if some files are installed outside $prefix.
# Check for a prefix ending in /_inst, if this is found, we can assume this
# to be a make distcheck, and disable some of the installcheck stuff.
AS_CASE([$prefix], [*/_inst],
    [AC_MSG_NOTICE([[Prefix ends in /_inst; this looks like a 'make distcheck']])
    is_make_distcheck=yes])
AM_CONDITIONAL([IS_MAKE_DISTCHECK], [test "x$is_make_distcheck" = xyes])
AC_MSG_CHECKING([final decision IS_MAKE_DISTCHECK (running "make distcheck"?)])
AM_COND_IF([IS_MAKE_DISTCHECK], [AC_MSG_RESULT([yes])], [AC_MSG_RESULT([no])])

# udev support
AX_PKG_CHECK_MODULES([UDEV], [udev], [], [have_udev=yes], [have_udev=no])
AM_CONDITIONAL([HAVE_UDEV], [test "x$have_udev" = xyes])
AC_ARG_WITH([udevrulesdir],
    AS_HELP_STRING([--with-udevrulesdir=DIR], [Directory for udev rules]),
    [udevrulesdir=$withval],
    [udevrulesdir=$($PKG_CONFIG --variable=udevdir udev)"/rules.d"])
AC_SUBST([udevrulesdir], [$udevrulesdir])

# Doxygen Support
AC_CHECK_PROGS([DOXYGEN], [doxygen])
AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])
AM_COND_IF([HAVE_DOXYGEN],
    [AC_CONFIG_FILES([Doxyfile])],
    [AC_MSG_WARN(["Doxygen not found; continuing without doxygen support"])])

# Configuration headers
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([ po/Makefile.in
    Makefile
    lib/Makefile
    lib/libx52/Makefile
    lib/libx52/libx52.pc
    lib/libusbx52/Makefile
    lib/libx52util/Makefile
    udev/Makefile
    utils/Makefile
    utils/cli/Makefile
    utils/test/Makefile
    tests/Makefile
])
AC_OUTPUT
