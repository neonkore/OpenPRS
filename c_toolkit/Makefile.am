#include ../site.make

AM_CPPFLAGS = -I$(top_builddir)/src  -I$(top_srcdir)/src 

C_TOOLKIT_LIB_SRC = mallocPack.c slistPack.c shashPack.c


C_TOOLKIT_LIB_INC =  slistPack.h shashPack.h listPack.h hashPack.h 

C_TOOLKIT_LIB_PUB_INC =  slistPack-pub.h shashPack-pub.h 

C_TOOLKIT_LIB_FINC = slistPack_f.h shashPack_f.h mallocPack_f.h


lib_LTLIBRARIES = libOPList.la

libOPList_la_SOURCES =  $(C_TOOLKIT_LIB_SRC) $(C_TOOLKIT_LIB_INC) $(C_TOOLKIT_LIB_FINC)

pkginclude_HEADERS = $(C_TOOLKIT_LIB_PUB_INC) $(C_TOOLKIT_LIB_INC) $(C_TOOLKIT_LIB_FINC)
