#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.53)

AC_INIT([Open PRS],[1.1b9],[felix@laas.fr],[openprs])
AC_CONFIG_SRCDIR([src/oprs-main.c])
AC_CONFIG_HEADERS([src/config-ac.h])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])

AC_LANG(C)

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_LIBTOOL_DLOPEN
# AC_LIBLTDL_CONVENIENCE
AC_PROG_LIBTOOL

LT_CONFIG_LTDL_DIR([libltdl])

LT_INIT([dlopen])
LTDL_INIT([convenience])

AC_SUBST(INCLTDL)
AC_SUBST(LIBLTDL)

AC_PROG_LN_S
AC_PROG_MAKE_SET

AM_INIT_AUTOMAKE([foreign])

AC_ARG_ENABLE([gtk],
  [  --enable-gtk    Turn on gtk based goprs/gope],
  [case "${enableval}" in
     yes) gtk=yes ;;
     no)  gtk=no ;;
      *) AC_MSG_ERROR([bad value ${enableval} for --enable-gtk]) ;;
    esac],[gtk=no])
AM_CONDITIONAL([WANT_GTK], [test x$gtk = xyes])

AC_ARG_ENABLE([demo],
  [  --enable-demo    Turn on demo based goprs/gope],
  [case "${enableval}" in
     yes) demo=yes ;;
     no)  demo=no ;;
      *) AC_MSG_ERROR([bad value ${enableval} for --enable-demo]) ;;
    esac],[demo=no])
AM_CONDITIONAL([WANT_DEMO], [test x$demo = xyes])

# we do not want it for now
AM_CONDITIONAL([WANT_OPC], [test xno = xyes])

#AM_PROG_LEX
#AC_PROG_YACC

AC_PROG_LEX
if test x"$LEX" = x:; then
   AC_MSG_FAILURE([required lex/flex program could not be found on your system.])
fi

AC_PROG_YACC
set -- $YACC
if ! command -v $1 >/dev/null; then
   AC_MSG_FAILURE([required yacc/bison program could not be found on your system.])
fi


AC_ARG_VAR(LATEX, "LaTeX text formatting and typesetting")
AC_CHECK_PROGS(LATEX, [pdflatex latex], [:])
AC_CHECK_PROGS(BIBTEX, [bibtex], [:])
AC_CHECK_PROGS(MAKEINDEX, [makeindex], [:])

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [sin])


AC_C_BIGENDIAN
AC_C_INLINE

AC_CHECK_FUNC([gethostbyname], [], [
  AC_CHECK_LIB([nsl], [gethostbyname], [MP_EXTRA_LIBS="-lnsl"] , [
    AC_CHECK_LIB([bsd], [gethostbyname], [MP_EXTRA_LIBS="-lbsd"])
  ])
])

AC_CHECK_FUNC([connect], [], [
  AC_CHECK_LIB([socket], [connect], [MP_EXTRA_LIBS="$MP_EXTRA_LIBS -lsocket"])
])

[LIBS="$LIBS $MP_EXTRA_LIBS"]

AC_SUBST(MP_EXTRA_LIBS)

#check for Readline
AC_CHECK_READLINE

PKG_CHECK_MODULES(X11, x11)

PKG_CHECK_MODULES(XT, xt)

AC_FIND_MOTIF

if test x$gtk = xyes; then
   PKG_CHECK_MODULES(GTK, gtk+-2.0)
fi

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h locale.h netdb.h netinet/in.h stddef.h stdlib.h string.h sys/file.h sys/ioctl.h sys/param.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_TYPE_UID_T

# Checks for library functions.
AC_CHECK_LIB(pthread, pthread_create)
AC_FUNC_FORK
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_CHECK_FUNCS([abort sigaction setitimer valloc clock_gettime dup2 gethostname getpagesize gettimeofday pow realpath select socket strcasecmp strchr strerror strncasecmp strrchr setpgid lrand48 rand])

AC_CONFIG_FILES([
                 Makefile
                 src/Makefile
                 c_toolkit/Makefile
                 util/Makefile
                 contrib/Makefile
                 contrib/data/Makefile
                 contrib/src/Makefile
		 contrib/src/openprs-util.pc
                 data/Makefile
                 data/test/Makefile
                 doc/Makefile
                 src/openprs.pc
                 src/mp-openprs.pc
                 doc/oprs-macros.tex
		 src/config.h
		 demo/Makefile
		 demo/truck-demo/Makefile
		 demo/truck-demo/src/Makefile
		 demo/truck-demo/oprs/Makefile
		 demo/truck-demo/oprs/data/Makefile
                ])
AC_OUTPUT

