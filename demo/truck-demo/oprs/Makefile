########################### -*- Mode: Makefile -*- #########################
# Makefile -- 
#
# $Id$
# 
# Copyright (c) 1991-2003 Francois Felix Ingrand.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#    - Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#    - Redistributions in binary form must reproduce the above
#      copyright notice, this list of conditions and the following
#      disclaimer in the documentation and/or other materials provided
#      with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#############################################################################

# Where OPRS is installed
# OPRS_INSTALLED_DIR = $(DESTDIR) Put this in your site.make

#OPRS_INSTALLED_DIR = /opt/oprs/oprs-1.5

#include $(OPRS_INSTALLED_DIR)/$(TARGET)/site.make
include ../../../path.make
include ../../../site.make

#
# directories
#
BIN_DIR = ../bin

ifdef OPRS_INSTALLED_DIR
OPRS_INCL_DIR = ${OPRS_INSTALLED_DIR}/include
OPRS_LIBS_DIR = $(OPRS_INSTALLED_DIR)/$(TARGET)/lib
UTIL_DIR = ${OPRS_INSTALLED_DIR}/util
else
OPRS_INCL_DIR = $(srctop)
OPRS_LIBS_DIR = ../../..
UTIL_DIR = ../../../util
endif

#
# Libraries
#
LIB_UTIL = ../bin/libutil.a

USER_SRC = user-evaluable-predicate.c user-action.c user-evaluable-function.c \
	user-external.c user-trace.c

USER_OBJ = user-evaluable-predicate.o user-action.o user-evaluable-function.o \
	user-external.o user-trace.o

MAKEFILE = Makefile

SRCS = $(USER_SRC)

INCLUDE = -I$(OPRS_INCL_DIR) -I$(OPRS_INCL_DIR)/c_toolkit

XDEFAULT =


CFLAGS = $(MT_CFLAGS) $(ANSI) $(DEBUG_FLAG) $(OPTIMIZE) $(WARNINGS) $(PROFILE)

CPPFLAGS = $(MT_CPPFLAGS) $(MACHINE) $(LANG_FLAG) $(INCLUDE) $(OTHER_INCLUDE) $(X_INCLUDE)

LDFLAGS = $(MT_LDFLAGS) $(ANSI) $(STATIC) $(DEBUG_FLAG) $(OPTIMIZE) $(PROFILE)


LIST_LIB = -L$(OPRS_LIBS_DIR)
SYS_LIB = -lm
LIBS = $(LIST_LIB) $(SYS_LIB) $(EXTRA_LIB)

# All the executables

all: oprs xoprs

oprs: $(OPRS_LIBS_DIR)/oprs-relocatable $(USER_OBJ) $(LIB_UTIL)
	 $(CC) -o oprs $(LDFLAGS) $(USER_OBJ)  $(OPRS_LIBS_DIR)/oprs-relocatable $(LIB_UTIL) $(LIBS)

xoprs:  $(OPRS_LIBS_DIR)/xoprs-relocatable $(USER_OBJ) $(LIB_UTIL)
	$(CC) -o xoprs $(LDFLAGS) $(USER_OBJ) $(OPRS_LIBS_DIR)/xoprs-relocatable $(LIB_UTIL) $(X_LIB) $(LIBS)

pxoprs:  $(OPRS_LIBS_DIR)/xoprs-relocatable $(USER_OBJ) $(LIB_UTIL)
	purify -chain-length="12" $(CC) -o pxoprs $(LDFLAGS) $(USER_OBJ) $(OPRS_LIBS_DIR)/xoprs-relocatable $(LIB_UTIL) $(X_LIB) $(LIBS)

$(LIB_UTIL):
	cd ../src ; make $(LIB_UTIL)

#
#The VxWorks version.
#
vxoprs: $(OPRS_LIBS_DIR)/vxoprs-relocatable $(USER_OBJ) ../src/time.o
	$(LD) -r -o vxoprs $(USER_OBJ) ../src/time.o $(OPRS_LIBS_DIR)/vxoprs-relocatable


#
# Some rules
#
(%.o): %.c
	$(CC) -c $(CFLAGS) $<

clean:
	/bin/rm -f core a.out gmon.out *.o make.depend.tmp make.depend.bak

very-clean: clean
	/bin/rm -f oprs xoprs vxoprs

#
# If the make.depend does not exist, just touch it.
#

make.depend:
	touch make.depend

depend:
	touch make.depend.tmp
	$(MKDEP)  $(CPPFLAGS) $(USER_SRC) > make.depend
#	makedepend -f make.depend -- $(CFLAGS) -- $(USER_SRC)

#
# This file contains the dependencies genrated by makedepend.
#
include make.depend
