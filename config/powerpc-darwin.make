# $Id$


GNUHOME = /usr
# Which compiler to use (gcc is prefered).
CC = cc

YACC = bison
YFLAGS = -d
OPRS_LDFLAGS = # -multiply_defined warning # -Wl,-force_flat_namespace
LEX = flex
LLIB = -ll
MV = mv
PERL = /usr/bin/perl

MKDEP 		= /Users/felix/bin/mkdep

DVIPS = /opt/tex/bin/dvips

LD = ld
RANLIB = ranlib
PURIFY =
OPTIMIZE = -O
DEBUG_FLAG = -g
STATIC =
INSTALL_STATIC =
ANSI = 
WARNINGS = -Wall

MEMLIB_DEB = -DDO_CHECK=0

# Where are the X11 and Motif libs on this machine.
X_LIB = -L/usr/X11R6/lib -L/sw/lib -lXm -lXp -lXt -lX11 -lXmu -lXext -lSM -lICE

# and the X include.
X_INCLUDE = -I/usr/X11R6/include -I/sw/include

EXTRA_LIB = -L/sw/lib -lreadline


DESTDIR = $(srctop)/..

OPRS_LANG = fr

USE_MULTI_THREAD = 1

ifdef USE_MULTI_THREAD
MT_LDFLAGS =
MT_CFLAGS =
endif
