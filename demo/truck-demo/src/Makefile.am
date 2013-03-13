########################### -*- Mode: Makefile -*- #########################
# Makefile.am -- 
#
# Copyright (c) 2013 Francois Felix Ingrand - LAAS/CNRS
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


AM_CPPFLAGS = -I$(top_srcdir)/c_toolkit
AM_CFLAGS = -Wall # -Wextra

AD2C = $(top_srcdir)/util/ad2c
ADLANG2AD = $(top_srcdir)/util/adlang2ad

bin_PROGRAMS = truck-demo

truck_demo_SOURCES = truck-demo.c  text-windows.c util.c oprs-interface.c message.c \
	clock-bboard.c demo-time.c valve.c filling.c \
	cmd-bboard.c cont-bboard.c fill-bboard.c error-bboard.c time.c \
	\
	demo.h	constant.h bitmaps.h \
	demo-time.h demo-type.h interface.h \
	local-time_f.h message.h \
	\
	 demo_f.h  local-time_f.h

truck_demo_CPPFLAGS = $(AM_CPPFLAGS) $(XT_CFLAGS) $(MOTIF_CFLAGS)
truck_demo_LDADD = $(MOTIF_LIBS) $(XT_LIBS) $(top_builddir)/c_toolkit/libOPList.la  $(top_builddir)/src/libExtMP.la 

# The X resources files
XDEFAULT = Truck-demo.adlang
EXTRA_DIST = $(XDEFAULT)

BUILT_SOURCES = Truck-demo.ad.h

xdefaultdir = $(libdir)/app-defaults
xdefault_DATA = Truck-demo.ad

%.ad.h: %.ad $(AD2C)
	$(AD2C) $< > $@

%.ad: %.adlang $(ADLANG2AD)
	rm -f $@
	$(ADLANG2AD) en < $< > $@
	chmod -w $@


#LIST_LIB = -L$(OPRS_LIBS_DIR) -lOprsMP
