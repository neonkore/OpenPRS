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


AM_CPPFLAGS  = -I$(top_srcdir)/src -I$(top_srcdir)/c_toolkit
AM_CFLAGS = -Wall # -Wextra

AD2C = $(top_srcdir)/util/ad2c
ADLANG2AD = $(top_srcdir)/util/adlang2ad


lib_LTLIBRARIES = libtruck-demo-time.la 

libtruck_demo_time_la_SOURCES = time.c

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

CLEANFILES =  Truck-demo.ad.h Truck-demo.ad

EXTRA_DIST += td-icon.bit bitmaps/armed-stop.bm bitmaps/bp.bm bitmaps/canvas100.bm \
	 bitmaps/disarm-stop.bm bitmaps/filled-tank0.bm bitmaps/filled-tank10.bm \
	bitmaps/filled-tank1.bm bitmaps/filled-tank2.bm bitmaps/filled-tank3.bm \
	bitmaps/filled-tank4.bm bitmaps/filled-tank5.bm bitmaps/filled-tank6.bm \
	bitmaps/filled-tank7.bm bitmaps/filled-tank8.bm bitmaps/filled-tank9.bm \
	bitmaps/filling-no-truck.bm bitmaps/leaving-truck.bm bitmaps/leaving-truck-filled.bm \
	bitmaps/light.bm bitmaps/light-off.bm bitmaps/light-on.bm bitmaps/ready-truck.bm \
	bitmaps/ready-truck-filled.bm bitmaps/tank0.bm bitmaps/tank10.bm bitmaps/tank1.bm \
	bitmaps/tank2.bm bitmaps/tank3.bm bitmaps/tank4.bm bitmaps/tank5.bm bitmaps/tank6.bm \
	bitmaps/tank7.bm bitmaps/tank8.bm bitmaps/tank9.bm bitmaps/truck.bm bitmaps/truck-filled.bm \
	bitmaps/valve.bm bitmaps/valve-open.bm



