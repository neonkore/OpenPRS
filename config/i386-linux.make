# $Id$


# Which compiler to use (gcc is prefered).
CC = gcc

YACC = bison
YFLAGS = -d
LEX = flex
LLIB = -lfl
LFLAGS =
MV = mv
PERL = /usr/bin/perl

MKDEP 		= /usr/local/robots/bin/$(TARGET)/mkdep
#MKDEP 		= $(HOME)/robots/bin/$(TARGET)/mkdep

DVIPS = /opt/tex/bin/dvips

LD = ld
RANLIB = true
PURIFY =
OPTIMIZE = -O
DEBUG_FLAG = -g
STATIC =
INSTALL_STATIC =
ANSI = 
WARNINGS = -Wall

MEMLIB_DEB = -DDO_CHECK=0

# Where are the X11 and Motif libs on this machine.
X_LIB = -L/usr/X11R6/LessTif/Motif1.2/lib -L/usr/X11R6/lib -lXm  -lXt -lX11 -lXmu -lXext -lXp

# and the X include.
X_INCLUDE = -I/usr/X11R6/LessTif/Motif1.2/include -I/usr/X11R6/include

EXTRA_LIB = -L/lib -lreadline -lhistory -ltermcap -lm

DESTDIR = $(srctop)/..

PRPC_LANG = en

USE_MULTI_THREAD = 1

ifdef USE_MULTI_THREAD
MT_LDFLAGS = -pthread
MT_CPPFLAGS = -pthread
endif

