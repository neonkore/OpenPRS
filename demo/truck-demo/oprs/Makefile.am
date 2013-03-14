########################### -*- Mode: Makefile -*- #########################
# Makefile.am -- 
# 
# Copyright (c) 2013 Francois Felix Ingrand.
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

SUBDIRS = data


AM_CPPFLAGS  = -I$(top_srcdir)/src -I$(top_srcdir)/c_toolkit

AM_CFLAGS = -Wall # -Wextra

lib_LTLIBRARIES = truck-demo-oprs.la 

TRUCK_DEMO_OPRS_MOD_SRC = user-evaluable-predicate.c user-action.c user-evaluable-function.c \
	user-external.c ../src/time.c

TRUCK_DEMO_OPRS_MOD_INCLUDE = user-external.h user-external_f.h

truck_demo_oprs_la_SOURCES = $(TRUCK_DEMO_OPRS_MOD_SRC)  $(TRUCK_DEMO_OPRS_MOD_INCLUDE)
truck_demo_oprs_la_LIBADD = 
truck_demo_oprs_la_LDFLAGS = -module 

pkginclude_HEADERS = $(TRUCK_DEMO_OPRS_MOD_INCLUDE)

