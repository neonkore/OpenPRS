#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.53)
AC_INIT(Open PRS, 1.0b, felix@laas.fr, OpenPRS)
AC_CONFIG_SRCDIR([oprs-main.c])
AC_CONFIG_HEADERS([config-ac.h config.h])
AC_CONFIG_AUX_DIR([util])

AC_LANG(C)

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_RANLIB
AC_PROG_LN_S
AC_PROG_MAKE_SET

dnl check for mkdep
AC_PATH_PROG(MKDEP, mkdep, no, $prefix/bin:$exec_prefix/bin:$PATH)
AC_PATH_PROG(BISON, bison, no, $prefix/bin:$exec_prefix/bin:$PATH)
AC_PATH_PROG(FLEX, flex, no, $prefix/bin:$exec_prefix/bin:$PATH)

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [sin])


AC_C_BIGENDIAN
AC_C_INLINE

AC_CHECK_FUNCS(gethostbyname, [], [
  AC_CHECK_LIB(nsl, gethostbyname)
])
AC_CHECK_LIB(socket, socket)

#check for Readline
AC_CHECK_READLINE

# Checks for header files.
AC_PATH_XTRA
AC_FIND_MOTIF

AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h locale.h malloc.h netdb.h netinet/in.h stddef.h stdlib.h string.h sys/file.h sys/ioctl.h sys/param.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_TYPE_UID_T

# Checks for library functions.
AC_FUNC_FORK
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_CHECK_FUNCS([abort sigaction setitimer valloc clock_gettime dup2 gethostname getpagesize gettimeofday pow realpath select socket strcasecmp strchr strerror strncasecmp strrchr setpgid])

AC_CONFIG_FILES([
		 Makefile
		 site.make
                 c_toolkit/Makefile
                 contrib/Makefile
                 contrib/data/Makefile
                 contrib/src/Makefile
                 data/Makefile
                 doc/Makefile
                 util/Makefile
                 stamp-h
		])
AC_OUTPUT

#                 demo/Makefile
#                 demo/truck-demo/Makefile
#                 demo/truck-demo/include/Makefile
#                 demo/truck-demo/oprs/Makefile
#                 demo/truck-demo/src/Makefile
#                 dist/Makefile
