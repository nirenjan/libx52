# Autoconf settings for pinelog
#
# Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: MIT

AC_INIT([pinelog], [1.0.0], [nirenjan@nirenjan.org])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall foreign subdir-objects])
AC_REQUIRE_AUX_FILE([tap-driver.sh])
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_AWK
AM_PROG_AR
LT_INIT
PKG_PROG_PKG_CONFIG
PKG_INSTALLDIR
AX_COMPILER_FLAGS
AX_GCC_FUNC_ATTRIBUTE([constructor])
AX_GCC_FUNC_ATTRIBUTE([destructor])
AX_GCC_FUNC_ATTRIBUTE([format])

AC_SUBST([PINELOG_CFLAGS])

# Configuration headers
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
