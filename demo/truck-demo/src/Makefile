########################### -*- Mode: Makefile -*- #########################
# Makefile -- 
# 
# $Id$
#
# Copyright (c) 1991-2009 Francois Felix Ingrand.
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
# ############################################################################

# Where OPRS is installed
# OPRS_INSTALLED_DIR = $(DESTDIR) Put this in your site.make

#OPRS_INSTALLED_DIR = /opt/oprs/oprs-1.5

#include $(OPRS_INSTALLED_DIR)/$(TARGET)/site.make
#include ../../../path.make
include ../../../site.make

#
# directories
#
BIN_DIR = ../bin
INCL_DIR = ../include


ifdef OPRS_INSTALLED_DIR
OPRS_INCL_DIR = ${OPRS_INSTALLED_DIR}/include
OPRS_LIBS_DIR = $(OPRS_INSTALLED_DIR)/$(TARGET)/lib
UTIL_DIR = ${OPRS_INSTALLED_DIR}/util
else
OPRS_INCL_DIR = $(srctop)
OPRS_LIBS_DIR = ../../..
UTIL_DIR = $(srctop)/util
endif

#
# Local Libraries
#
LIB_LOCAL	= $(BIN_DIR)/libdemo.a
LIB_UTIL	= $(BIN_DIR)/libutil.a

# All the C sources
LIB_LOCAL_SRC = truck-demo.c  text-windows.c util.c oprs-interface.c message.c \
	clock-bboard.c demo-time.c valve.c filling.c \
	cmd-bboard.c cont-bboard.c fill-bboard.c error-bboard.c

LIB_UTIL_SRC = time.c


LIB_LOCAL_OBJ = $(LIB_LOCAL_SRC:%.c=%.o)

LIB_UTIL_OBJ = $(LIB_UTIL_SRC:%.c=%.o)

# All the .h include files
INC = ../include/demo.h	../include/constant.h ../include/bitmaps.h\
	../include/demo-time.h ../include/demo-type.h ../include/interface.h\
	../include/local-time_f.h ../include/message.h

#All the _f.h function include
FINC = ../include/demo_f.h  ../include/local-time_f.h

# The X resources files
RESOURCES_SRC = Truck-demo.adlang

RESOURCES_INC_FILES = Truck-demo.ad.h

XDEFAULT_LANG = Truck-demo-en.ad Truck-demo-fr.ad

MAKEFILE = Makefile

SRCS = $(LIB_LOCAL_SRC) $(LIB_UTIL_SRC) $(INC) $(FINC)

INCLUDE =  -I$(INCL_DIR) -I$(OPRS_INCL_DIR) -I$(OPRS_INCL_DIR)/c_toolkit

XDEFAULT =

LANG_FLAG = -DLANGUAGE=$(LANG)


CFLAGS = $(MT_CFLAGS) $(ANSI) $(DEBUG_FLAG) $(OPTIMIZE) $(WARNINGS) $(PROFILE)

CPPFLAGS = $(MT_CPPFLAGS) $(MACHINE) $(LANG_FLAG) $(INCLUDE) $(OTHER_INCLUDE) $(X_INCLUDE)

LDFLAGS = $(MT_LDFLAGS) $(ANSI) $(STATIC) $(DEBUG_FLAG) $(OPTIMIZE) $(PROFILE)

LIST_LIB = -L$(OPRS_LIBS_DIR) -lOprsMP
SYS_LIB = -lm
LIBS = $(LIST_LIB) $(SYS_LIB) $(EXTRA_LIB)

LD = ld

# All the executables

PROG = $(BIN_DIR)/truck-demo

all: $(PROG)

$(BIN_DIR)/truck-demo: $(BIN_DIR) $(LIB_UTIL) $(LIB_LOCAL)
	$(CC) $(LDFLAGS) -o $(BIN_DIR)/truck-demo $(LIB_LOCAL) $(LIB_UTIL) \
	$(X_LIB) $(LIBS)

$(LIB_LOCAL): $(LIB_LOCAL_OBJ:%=$(LIB_LOCAL)(%))
	$(RANLIB) $@

$(LIB_UTIL): $(LIB_UTIL_OBJ:%=$(LIB_UTIL)(%))
	$(RANLIB) $@


#
# This rule says that the .ad.h are obtained with the coresponding .ad
# I add util/ad2c in case it is not there... to avoid emptying the file.
#
%.ad.h: %.ad $(UTIL_DIR)/ad2c
	$(UTIL_DIR)/ad2c $< > $@

%-fr.ad: %.adlang $(UTIL_DIR)/adlang2ad
	rm -f $@
	$(UTIL_DIR)/adlang2ad fr < $< > $@
# To make sure we do not modify these...
	chmod -w $@

%-en.ad: %.adlang $(UTIL_DIR)/adlang2ad
	rm -f $@
	$(UTIL_DIR)/adlang2ad en < $< > $@
# To make sure we do not modify these...
	chmod -w $@

ifeq ($(LANG),fr)
%.ad: %-fr.ad
	rm -f $@
	ln -s $< $@
else
%.ad: %-en.ad
	rm -f $@
	ln -s $< $@
endif

clean:
	/bin/rm -f core a.out gmon.out *.o make.depend.tmp make.depend.tmp.bak

very-clean: clean
	/bin/rm -f $(PROG) $(LIB_LOCAL) $(LIB_UTIL) TAGS $(OLD_FILES)

OLD_FILES = ../include/color.h simulator.c

#
# Make depend stuff
#
# If the make.depend does not exist, just touch it.
#

make.depend:
	@echo "*******************************"
	@echo "*********** WARNING ***********"
	@echo "*******************************"
	@echo "make.depend does not exist."
	@echo "I will create it for you."
	@echo "Run make depend to fill it."
	touch make.depend

depend: Truck-demo.ad.h
	${UTIL_DIR}/mkdep $(CPPFLAGS) \
		 -l$(LIB_LOCAL) $(LIB_LOCAL_SRC) \
		 -l$(LIB_UTIL) $(LIB_UTIL_SRC) \
		> make.depend


#
# This file contains the dependencies genrated by makedepend.
#
include make.depend

#
# Misc
#

# Just to make sure the directory exists (most likely it will not the first time the user will type make... 
# We better create it then).
$(BIN_DIR):
	mkdir $(BIN_DIR)

tags: TAGS

TAGS: $(SRCS)
	etags $(SRCS) $(RESOURCES_FILE) Makefile

