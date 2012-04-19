

AM_CPPFLAGS = -I$(top_srcdir)/c_toolkit -I$(top_builddir)/src  -I$(top_srcdir)/src 


lib_LTLIBRARIES = libpu.la

PU_LIB_SRC = pu-parse-tl.c pu-mk-term.c pu-enum.c pu-genom.c
PU_LIB_INC = $(PU_LIB_SRC:%.c=%_f.h) pu-enum.h

libpu_la_SOURCES =  $(PU_LIB_SRC)

pkginclude_HEADERS = $(PU_LIB_INC)


pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = openprs-util.pc
