dnl                                      -*- Autoconf -*-
dnl	$LAAS$
dnl
dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.53)

AC_INIT([myOprs], [1.0b0], [felix@laas.fr], [myprs])

AC_CONFIG_SRCDIR([src/evaluable-function.c])
AC_CONFIG_MACRO_DIR([m4])

PKG_CHECK_MODULES(OPENPRS, openprs >= 1.1)
PKG_CHECK_MODULES(OPENPRS_UTIL, openprs-util >= 1.1)

AC_CANONICAL_HOST

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

AM_INIT_AUTOMAKE([foreign])

# Checks for header files.
AC_HEADER_STDC

AC_CONFIG_FILES([
		 Makefile
		 pr2matutils.pc
		 src/Makefile
		])
AC_OUTPUT


