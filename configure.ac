AC_INIT([x52pro-linux], [0.1.0], [nirenjan@gmail.com])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_PROG_CC
AM_PROG_AR
LT_INIT

# Check for libusb-1.0
AC_SEARCH_LIBS([libusb_init], [usb-1.0], [have_libusb=yes])

if test "x${have_libusb}" = xyes; then
    AC_CHECK_HEADERS([libusb-1.0/libusb.h], [], [have_libusb=no])
fi

if test "x${have_libusb}" != xyes; then
    AC_MSG_ERROR([
    -------------------------------------------------------
     The library libusb-1.0 and header file libusb.h
     are required to build x52pro-linux. Stopping...
     Check 'config.log' for more information.
    -------------------------------------------------------
    ])
fi

AC_SUBST([X52_PKG_VERSION], [0.1])
AC_SUBST([X52_INCLUDE], ["-I \$(top_srcdir)/libx52"])
AC_SUBST([X52_CORE_LIB], [../libx52/libx52.la])

AC_CONFIG_FILES([
    Makefile
    libx52/Makefile
    test/Makefile
    cli/Makefile
    util/Makefile
])
AC_OUTPUT
