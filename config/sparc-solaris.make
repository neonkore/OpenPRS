# $Id$

GNUHOME = /usr/local/gnu

# Which compiler to use (gcc is prefered).
CC = cc

YACC = bison
YFLAGS = -d
LEX = flex

LLIB = -L /usr/local/gnu/lib -lfl
LFLAGS =
MV = mv

MKDEP 		= /usr/local/robots/bin/$(TARGET)/mkdep

DVIPS = /usr/local/tex/bin/dvips

LD = ld
RANLIB = true
PURIFY =
OPTIMIZE = 
DEBUG_FLAG = -g
STATIC =
INSTALL_STATIC =
ANSI = 
WARNINGS =

MEMLIB_DEB = -DDO_CHECK=0

# Where are the X11 and Motif libs on this machine.
X_LIB = -R${MOTIFHOME}/lib -L${MOTIFHOME}/lib -lXm  -R/usr/openwin/lib -L/usr/openwin/lib -lXt -lX11 -lXmu -lXext

# and the X include.
X_INCLUDE = -I/usr/openwin/include -I${MOTIFHOME}/include

EXTRA_LIB = -lsocket -lnsl -L/usr/ccs/lib -lgen -R$(GNUHOME)/lib -L$(GNUHOME)/lib -lreadline -lhistory -ltermcap -lm

DESTDIR = $(srctop)/..

PRPC_LANG = en

USE_MULTI_THREAD = 1

ifdef USE_MULTI_THREAD
MT_LDFLAGS = -lthread
MT_CPPFLAGS = -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS
endif
