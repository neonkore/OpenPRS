
AM_CPPFLAGS = -I$(top_srcdir)/c_toolkit -I$(top_srcdir)/src

lib_LTLIBRARIES = libpu.la

PU_LIB_SRC = pu-parse-tl.c pu-mk-term.c pu-enum.c pu-genom.c
PU_LIB_INC = $(PU_LIB_SRC:%.c=%_f.h)

libpu_la_SOURCES =  pu-parse-tl.c pu-mk-term.c pu-enum.c pu-genom.c

pkginclude_HEADERS = $(PU_LIB_INC)