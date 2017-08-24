AC_INIT([x52pro-linux], [0.2.0], [nirenjan@gmail.com])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_PROG_CC
AM_PROG_AR
AM_PATH_PYTHON([2.6])
LT_INIT

# Check for libusb-1.0
AC_SEARCH_LIBS([libusb_init], [usb-1.0], [have_libusb=yes])

AS_IF([test "x${have_libusb}" = xyes], [
    AC_CHECK_HEADERS([libusb-1.0/libusb.h], [], [have_libusb=no])
])

AS_IF([test "x${have_libusb}" != xyes], [
    AC_MSG_ERROR([
    -------------------------------------------------------
     The library libusb-1.0 and header file libusb.h
     are required to build x52pro-linux. Stopping...
     Check 'config.log' for more information.
    -------------------------------------------------------
    ])
])

AC_SUBST([X52_PKG_VERSION], [0.1])
AC_SUBST([X52_INCLUDE], ["-I \$(top_srcdir)/lib/libx52"])

AC_CONFIG_FILES([
    Makefile
    lib/Makefile
    lib/libx52/Makefile
    lib/libusbx52/Makefile
    lib/libx52util/Makefile
    utils/Makefile
    utils/cli/Makefile
    utils/test/Makefile
    tests/Makefile
])
AC_OUTPUT
