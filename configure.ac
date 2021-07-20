# Autoconf settings for x52pro-linux
#
# Copyright (C) 2012-2020 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0

AC_INIT([x52pro-linux], [0.2.1], [nirenjan@gmail.com])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall foreign subdir-objects])
AC_REQUIRE_AUX_FILE([tap-driver.sh])
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_AWK
AM_PROG_AR
AM_PATH_PYTHON([3.5])
LT_INIT
PKG_PROG_PKG_CONFIG
PKG_INSTALLDIR
AX_COMPILER_FLAGS
AC_CANONICAL_HOST
AX_GCC_FUNC_ATTRIBUTE([constructor])
AX_GCC_FUNC_ATTRIBUTE([destructor])
AX_GCC_FUNC_ATTRIBUTE([format])
AX_GCC_FUNC_ATTRIBUTE([noreturn])

AC_MSG_NOTICE([Detected host OS is ${host_os}])
build_linux=no
# Detect target system
case "${host_os}" in
    linux*)
        build_linux=yes
        ;;
esac
AM_CONDITIONAL([LINUX], [test "x${build_linux}" = "xyes"])

# Internationalization
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION(0.19)
AM_CONDITIONAL([USE_NLS], [test "x${USE_NLS}" == "xyes"])

# Check for libusb-1.0
AX_PKG_CHECK_MODULES([LIBUSB], [libusb-1.0])

AC_SUBST([LIBUSB_CFLAGS])
AC_SUBST([LIBUSB_LDFLAGS])
AC_SUBST([LIBUSB_LIBS])

# Check for libinih
PKG_CHECK_MODULES([INIH], [inih], [],
                  [
                    # Older versions of Ubuntu don't provide a .pc file
                    AC_CHECK_HEADERS([ini.h])
                    AC_SEARCH_LIBS([ini_parse], [inih])
                  ])
AC_SUBST([INIH_CFLAGS])
AC_SUBST([INIH_LDFLAGS])
AC_SUBST([INIH_LIBS])

# Pinelog configuration
AC_SUBST([PINELOG_CFLAGS], ["-DPINELOG_SHOW_DATE=1 -DPINELOG_SHOW_LEVEL=1 -DPINELOG_SHOW_BACKTRACE=1"])

AC_SUBST([X52_INCLUDE], ["-I \$(top_srcdir)/lib/libx52"])

# Check for hidapi. This uses a different pkg-config file on Linux vs other
# hosts, so check accordingly
AM_COND_IF([LINUX], [hidapi_backend=hidapi-hidraw], [hidapi_backend=hidapi])
PKG_CHECK_MODULES([HIDAPI], [${hidapi_backend}])
AC_SUBST([HIDAPI_CFLAGS])
AC_SUBST([HIDAPI_LDFLAGS])
AC_SUBST([HIDAPI_LIBS])

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
PKG_CHECK_MODULES([UDEV], [udev], [have_udev=yes], [have_udev=no])
AM_CONDITIONAL([HAVE_UDEV], [test "x$have_udev" = xyes])
AC_ARG_WITH([udevrulesdir],
    AS_HELP_STRING([--with-udevrulesdir=DIR], [Directory for udev rules]),
    [udevrulesdir=$withval],
    [udevrulesdir=$($PKG_CONFIG --variable=udevdir udev)"/rules.d"])
AC_SUBST([udevrulesdir], [$udevrulesdir])

AC_ARG_WITH([input-group],
    AS_HELP_STRING([--with-input-group=GROUP], [Group allowed to access input devices]),
    [input_group=$withval],
    [input_group=plugdev])
AC_SUBST([input_group], [$input_group])

# Doxygen Support
AC_CHECK_PROGS([DOXYGEN], [doxygen])
AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])
AM_COND_IF([HAVE_DOXYGEN],
    [AC_CONFIG_FILES([Doxyfile])],
    [AC_MSG_WARN(["Doxygen not found; continuing without doxygen support"])])

# cmocka unit tests
PKG_CHECK_MODULES([CMOCKA], [cmocka >= 1.1], [have_cmocka=yes], [have_cmocka=no])
AM_CONDITIONAL([HAVE_CMOCKA], [test "x$have_cmocka" = xyes])
AM_COND_IF([HAVE_CMOCKA], [],
    [AC_MSG_WARN(["cmocka not found; disabling unit test build"])])

# Check for the presence of tm_gmtoff in struct tm. If we have this, then we
# can use it to determine the true GMT offset
AC_CHECK_MEMBERS([struct tm.tm_gmtoff],,
    [AC_MSG_WARN(["Cannot find tm_gmtoff in struct tm, using slower method"])],
    [#define _GNU_SOURCE
     #include <time.h>
    ])

# Configuration headers
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([ po/Makefile.in
    Makefile
    lib/Makefile
    lib/libx52/Makefile
    lib/libx52/libx52.pc
    lib/libusbx52/Makefile
    lib/libx52util/Makefile
    lib/libx52io/Makefile
    lib/pinelog/Makefile
    udev/Makefile
    udev/60-saitek-x52-x52pro.rules
    utils/Makefile
    utils/cli/Makefile
    utils/test/Makefile
    utils/evtest/Makefile
    daemon/Makefile
    tests/Makefile
])
AC_OUTPUT
