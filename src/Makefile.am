#
# Copyright (c) 2012-2013 LAAS/CNRS
# All rights reserved.
#
# Redistribution  and  use in source   and binary forms,  with or without
# modification, are permitted provided that  the following conditions are
# met:
#
#   1. Redistributions  of  source code must  retain  the above copyright
#      notice, this list of conditions and the following disclaimer.
#   2. Redistributions in binary form must  reproduce the above copyright
#      notice,  this list of  conditions and  the following disclaimer in
#      the  documentation   and/or  other  materials   provided with  the
#      distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE  AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY  EXPRESS OR IMPLIED WARRANTIES, INCLUDING,  BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES   OF MERCHANTABILITY AND  FITNESS  FOR  A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO  EVENT SHALL THE AUTHOR OR  CONTRIBUTORS
# BE LIABLE FOR ANY DIRECT, INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING,  BUT  NOT LIMITED TO, PROCUREMENT  OF
# SUBSTITUTE  GOODS OR SERVICES;  LOSS   OF  USE,  DATA, OR PROFITS;   OR
# BUSINESS  INTERRUPTION) HOWEVER CAUSED AND  ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE  USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

AM_CPPFLAGS = -I$(top_srcdir)/c_toolkit
AM_CFLAGS = -Wall # -Wextra

#
# I am too lazy to add these in the various automake SOURCES... sorry.
#
INC = action.h activate.h conditions.h config.h constant-pub.h \
	constant.h database.h default-hook.h ev-function.h ev-predicate.h \
	fact-goal.h gope-global.h gope-graphic.h goprs-intention.h \
	goprs-main-pub.h goprs-main.h goprs.h int-graph.h intend.h \
	intention.h lang.h lisp-list.h macro-pub.h macro.h mp-pub.h \
	mp-register.h mp.h op-compiler.h op-default.h op-instance.h \
	op-structure.h opaque-ext.h opaque-pub.h opaque.h opc.h \
	ope-global.h ope-gope.h ope-graphic.h ope-syntax.h oprs-client.h \
	oprs-dump.h oprs-error-pub.h oprs-error.h oprs-main.h \
	oprs-pred-func.h oprs-print.h oprs-profiling.h oprs-rerror.h \
	oprs-server.h oprs-sprint-pub.h oprs-sprint.h oprs-type-pub.h \
	oprs-type.h oprs.h parser-funct.h relevant-op.h soak.h tcl.h \
	top-lev.h type.h unification.h user-action.h user-ev-function.h \
	user-ev-predicate.h user-trace.h version.h xhelp.h xm2gtk.h \
	xoprs-intention.h xoprs-main-pub.h xoprs-main.h xoprs-textwin.h \
	xoprs.h xoprs.h yy-funct.c

FINC = action_f.h activate_f.h conditions_f.h database_f.h \
	default-hook_f.h ev-function_f.h ev-predicate_f.h \
	fact-goal_f-pub.h fact-goal_f.h gope-bboard_f.h gope-edit_f.h \
	gope-external_f.h gope-filesel_f.h gope-graphic_f.h gope-main_f.h \
	gope-menu_f.h gope-op-opf_f.h gope-report_f.h gope-vsblf_f.h \
	goprs-intention_f.h goprs-menu_f.h goprs-textwin_f.h gtop-lev_f.h \
	help_f.h int-graph_f-pub.h int-graph_f.h intend_f.h \
	intention_f-pub.h intention_f.h lisp-list_f-pub.h lisp-list_f.h \
	ltdl-loader_f.h mp-register_f.h op-compiler_f.h \
	op-instance_f-pub.h op-instance_f.h op-structure_f.h \
	ope-bboard_f.h ope-edit_f.h ope-external_f.h ope-filesel_f.h \
	ope-graphic_f.h ope-menu_f.h ope-op-opf_f.h ope-print_f.h \
	ope-pxmw_f.h ope-report_f.h ope-rop_f.h ope-save_f.h \
	ope-syntax_f.h ope-vsblf_f.h oprs-array_f.h oprs-client_f.h \
	oprs-dump_f.h oprs-error_f.h oprs-init_f.h oprs-pprint_f.h \
	oprs-pred-func_f.h oprs-print_f-pub.h oprs-print_f.h \
	oprs-profiling_f.h oprs-rerror_f-pub.h oprs-rerror_f.h \
	oprs-send-m_f.h oprs-socket_f.h oprs-sprint_f-pub.h \
	oprs-sprint_f.h oprs-type_f-pub.h oprs-type_f.h oprs-util_f.h \
	oprs_f-pub.h oprs_f.h other_proto.h parser-funct_f.h \
	relevant-op_f.h send-message_f.h soak_f.h socket_f.h tcl_f.h \
	tcl_f.h top-lev_f.h top-structure_f.h type_f.h unification_f.h usage_f.h \
	user-end-hook_f.h xhelp_f.h xm2gtk_f.h xoprs-dialog_f.h xoprs-intention_f.h \
	xoprs-menu_f.h xoprs-textwin_f.h xt-util_f.h yy-funct_f.h goprs-call_f.h \
	goprs-dialog_f.h goprs-filesel_f.h op-x-opaque_f.h op-dump-load_f.h \
	oprs-server-main_f.h xoprs-main_f.h



#
# These are the parts to make the various yacc/bison grammars needed by the various programs.
# See below how they are built with lex-includer.
#
YACC_PART = opc-parser.y.part oprs-parser.y.part opt-parser.y.part yacc-file.y.part \
	yacc-text-op.y.part ope-parser.y.part oprs-server-parser.y.part \
	yacc-exp.y.part yacc-graph-op.y.part


#
# These are the parts to make the various lex/flex grammars needed by the various programs.
# See below how they are built with lex-includer.
#
LEX_PART = lex-exp.l.part lex-macro.l.part lex-parser-def.l.part lex-text-op.l.part \
	ope-lex.l.part oprs-server-lex.l.part lex-graph-op.l.part lex-meta-exp.l.part \
	lex-simple-def.l.part opc-lex.l.part oprs-lex.l.part

# Icon files
# for X/Motif (.bit)
# for GTK (.png)
DIST_ANNEX_FILES = ope-icon.bit xoprs-icon.bit \
	gope-icon.png goprs-icon.png		 


# X Default files for X/Motif
XDEFAULT = XOprs.adlang Ope.adlang

EXTRA_DIST = $(INC) $(FINC) $(YACC_PART) $(LEX_PART) $(XDEFAULT) $(DIST_ANNEX_FILES)

EXTERNAL_INCLUDE = action_f-pub.h constant-pub.h ev-function_f-pub.h ev-predicate_f-pub.h \
	fact-goal_f-pub.h int-graph_f-pub.h intention_f-pub.h lang.h oprs-sprint-pub.h \
	op-instance_f-pub.h op-structure_f-pub.h lisp-list_f-pub.h macro-pub.h \
	mp-pub.h opaque-pub.h oprs-array_f-pub.h oprs-type-pub.h oprs-sprint_f-pub.h \
	oprs-type_f-pub.h oprs_f-pub.h user-action.h user-ev-function.h \
	user-ev-predicate.h xoprs-main-pub.h oprs-error-pub.h \
	oprs-rerror_f-pub.h user-trace.h user-trace_f.h oprs-print_f-pub.h \
	user-end-hook_f-pub.h user-end-hook-pub.h  default-hook.h default-hook_f.h

pkginclude_HEADERS = $(EXTERNAL_INCLUDE)

#
lib_LTLIBRARIES = libExtMP.la 
noinst_LTLIBRARIES = libMP.la libBasic.la libOprs.la \
		libTOprs.la libXGOprs.la libKernel.la \
		libTKernel.la libXGKernel.la \
		libXOpG.la 

if WANT_GTK
noinst_LTLIBRARIES += libGGOprs.la libGGKernel.la libGOpG.la
endif


noinst_LIBRARIES = libVersion.a


libVersion_a_SOURCES = version.c
libVersion_a_CPPFLAGS = $(AM_CPPFLAGS) -DCOMPIL_DATE="\"`date`\"" -DHOST="\"`hostname`\""

############################################
# message passer library
LIB_MP_SOURCES = mp-register.c send-message.c lang.c

# this is for external program... do not confuse them with our homebrew malloc.
libExtMP_la_SOURCES = $(LIB_MP_SOURCES)
libExtMP_la_CPPFLAGS = $(AM_CPPFLAGS) -DEXT_MALLOC

############################################
# this is for our own usage
libMP_la_SOURCES = $(LIB_MP_SOURCES)

############################################
# Library used by ALL the programs (mp-oprs included)
libBasic_la_SOURCES = oprs-socket.c oprs-util.c

############################################
# Library used by all the programs except the mp-oprs
libOprs_la_SOURCES = help.c lisp-list.c oprs-ll.c oprs-send-m.c oprs-sprint.c \
		parser-funct.c oprs-array.c oprs-copy.c oprs-dup.c oprs-free.c oprs-print.c \
		oprs-error.c op-compiler.c type.c oprs-pred-func.c

############################################
# Library used by all kernel programs (oprs text and graphic) but not graphic dependant.
libKernel_la_SOURCES = database.c ev-predicate.c fact-goal.c oprs.c oprs-rerror.c unification.c oprs-profiling.c ltdl-loader.c


############################################
# Library used by graphic programs {x,g}oprs and {,g}ope.
# X11 version
libXOpG_la_SOURCES = ope-external.c ope-graphic.c xhelp.c oprs-pprint.c ope-report.c xt-util.c
libXOpG_la_CPPFLAGS = $(AM_CPPFLAGS) -DGRAPHIX -UGTK $(XT_CFLAGS) $(MOTIF_CFLAGS)

# GTK version
libGOpG_la_SOURCES = gope-external.c gope-graphic.c xhelp.c oprs-pprint.c gope-report.c g-util.c
libGOpG_la_CPPFLAGS = $(AM_CPPFLAGS) -DGRAPHIX -DGTK $(GTK_CFLAGS)

############################################
# Library used by all kernel programs (oprs text and graphic) and
# editors (ope and gope) but graphic dependant, hence the 3 versions.
BOTH_OPRS_LIB_SOURCES = op-structure.c oprs-subst.c oprs-type.c conditions.c top-structure.c consult-rop.c relevant-op.c op-dump-load.c oprs-dump.c oprs-load.c

libTOprs_la_SOURCES = $(BOTH_OPRS_LIB_SOURCES)
libTOprs_la_CPPFLAGS = $(AM_CPPFLAGS) -UGRAPHIX

libXGOprs_la_SOURCES = $(BOTH_OPRS_LIB_SOURCES)
libXGOprs_la_CPPFLAGS = $(AM_CPPFLAGS) -DGRAPHIX -UGTK $(XT_CFLAGS) $(MOTIF_CFLAGS)

libGGOprs_la_SOURCES = $(BOTH_OPRS_LIB_SOURCES)
libGGOprs_la_CPPFLAGS = $(AM_CPPFLAGS) -DGRAPHIX -DGTK $(GTK_CFLAGS)

############################################
# Library used by all kernel programs (oprs text and graphic).
BOTH_KERNEL_LIB_SOURCES = action.c activate.c ev-function.c intend.c intention.c \
	int-graph.c op-instance.c oprs-init.c tcl.c soak.c \
	top-lev.c default-hook.c user-end-hook.c

libTKernel_la_SOURCES = $(BOTH_KERNEL_LIB_SOURCES)
libTKernel_la_CPPFLAGS = $(AM_CPPFLAGS) -I$(top_srcdir)/contrib/src -UGRAPHIX

libXGKernel_la_SOURCES = $(BOTH_KERNEL_LIB_SOURCES)
libXGKernel_la_CPPFLAGS = $(AM_CPPFLAGS) -I$(top_srcdir)/contrib/src -DGRAPHIX -UGTK $(XT_CFLAGS) $(MOTIF_CFLAGS)

libGGKernel_la_SOURCES = $(BOTH_KERNEL_LIB_SOURCES)
libGGKernel_la_CPPFLAGS = $(AM_CPPFLAGS) -I$(top_srcdir)/contrib/src -DGRAPHIX -DGTK $(GTK_CFLAGS)

#
# We know use ltdl to load these.
#
USER_MODULES_EXAMPLE = user-action.c user-ev-function.c user-ev-predicate.c

############################################
# this is an externaly installed library with many utility functions.
CONTRIB_LIB = $(top_builddir)/contrib/src/libpu.la

INCLUDES = @INCLTDL@

############################################
# Here is the list of program to build and install
bin_PROGRAMS = mp-oprs kill-mp oprs-server oprs xoprs oprs-cat ope 

if WANT_OPC
bin_PROGRAMS += opc
endif

# we do not install GTK version for now (too beta).
if WANT_GTK
bin_PROGRAMS += goprs gope
endif

mp_oprs_SOURCES = mp.c
mp_oprs_LDADD = ../c_toolkit/libOPList.la libBasic.la libMP.la libVersion.a

kill_mp_SOURCES = kill-mp.c
kill_mp_LDADD = ../c_toolkit/libOPList.la libBasic.la libMP.la libVersion.a

nodist_oprs_server_SOURCES = oprs-server-parser.y oprs-server-lex.l
oprs_server_SOURCES = opaque-ext.c oprs-client.c \
		oprs-server-main.c oprs-server-opaque.c
oprs_server_CPPFLAGS = $(READLINE_INCLUDES) $(AM_CPPFLAGS)
oprs_server_LDADD = ../c_toolkit/libOPList.la libBasic.la libTOprs.la libOprs.la libMP.la \
		 libVersion.a $(READLINE_LIBS)

nodist_oprs_SOURCES = oprs-lex.l oprs-parser.y
oprs_SOURCES =  op-x-opaque.c oprs-main.c top-lev-loop.c user-trace.c
oprs_LDFLAGS = -export-dynamic
oprs_LDADD = libTKernel.la libKernel.la libBasic.la libTOprs.la libOprs.la \
		 libMP.la libVersion.a ../c_toolkit/libOPList.la $(CONTRIB_LIB)\
		 @LIBLTDL@ @LIBADD_DL@

nodist_xoprs_SOURCES = oprs-lex.l  oprs-parser.y
xoprs_SOURCES = xoprs-call.c xoprs-textwin.c \
	xoprs-dialog.c xoprs-filesel.c xoprs-intention.c \
	xoprs-main.c xoprs-menu.c xtop-lev-loop.c \
	xoprs-op-graphic.c xoprs-rop.c user-trace.c
xoprs_CPPFLAGS = $(AM_CPPFLAGS) -DGRAPHIX -UGTK $(XT_CFLAGS) $(MOTIF_CFLAGS)
xoprs_LDFLAGS = -export-dynamic
xoprs_LDADD = libXGKernel.la libKernel.la libBasic.la libXGOprs.la libOprs.la \
	libXOpG.la libMP.la libVersion.a ../c_toolkit/libOPList.la $(CONTRIB_LIB) \
	$(MOTIF_LIBS) $(XT_LIBS) @LIBLTDL@ @LIBADD_DL@

nodist_goprs_SOURCES = oprs-lex.l oprs-parser.y
goprs_SOURCES = xm2gtk.c goprs-call.c goprs-textwin.c \
	goprs-dialog.c goprs-filesel.c goprs-intention.c \
	goprs-main.c goprs-menu.c gtop-lev-loop.c \
	goprs-op-graphic.c goprs-rop.c user-trace.c
goprs_CPPFLAGS = $(AM_CPPFLAGS) -DGRAPHIX -DGTK $(GTK_CFLAGS)
goprs_LDADD = libGGKernel.la libKernel.la libBasic.la libGGOprs.la libOprs.la \
	libGOpG.la libMP.la libVersion.a ../c_toolkit/libOPList.la $(CONTRIB_LIB) \
	@GTK_LIBS@ @LIBLTDL@ @LIBADD_DL@

oprs_cat_SOURCES = oprs-cat.c
oprs_cat_LDADD = ../c_toolkit/libOPList.la

nodist_ope_SOURCES = ope-lex.l ope-parser.y
ope_SOURCES = ope-bboard.c ope-edit.c ope-filesel.c ope-op-opf.c ope-main.c \
	ope-menu.c ope-print.c ope-pxmw.c ope-save.c ope-syntax.c \
	ope-vsblf.c ope-rop.c ope-op-str.c ope-opaque.c
ope_CPPFLAGS = $(AM_CPPFLAGS) -DGRAPHIX -UGTK $(XT_CFLAGS) $(MOTIF_CFLAGS)
ope_LDADD = libBasic.la libXGOprs.la libOprs.la \
		libXOpG.la libMP.la libVersion.a ../c_toolkit/libOPList.la $(MOTIF_LIBS) $(XT_LIBS)

nodist_gope_SOURCES = ope-lex.l ope-parser.y
gope_SOURCES = xm2gtk.c gope-bboard.c gope-edit.c gope-filesel.c gope-op-opf.c gope-main.c \
	gope-menu.c ope-save.c ope-syntax.c gope-vsblf.c gope-rop.c ope-op-str.c ope-opaque.c
gope_CPPFLAGS = $(AM_CPPFLAGS) -DGRAPHIX -DGTK $(GTK_CFLAGS)
gope_LDADD = libBasic.la libGGOprs.la libOprs.la \
		libGOpG.la libMP.la libVersion.a ../c_toolkit/libOPList.la @GTK_LIBS@

if WANT_OPC
nodist_opc_SOURCES = opc-lex.l opc-parser.y
opc_SOURCES =  opc-main.c ope-opaque.c
opc_CPPFLAGS = $(AM_CPPFLAGS) -DGRAPHIX -UGTK $(XT_FLAGS) $(MOTIF_CFLAGS)
opc_LDADD = libBasic.la libXGOprs.la libOprs.la \
	libXOpG.la libMP.la libVersion.a ../c_toolkit/libOPList.la $(MOTIF_LIBS) $(XT_LIBS)
endif

# This is what replace relocatable... dynamic loading of evaluable function, predicates and actions.

lib_LTLIBRARIES += user-ev-function.la
user_ev_function_la_SOURCES = user-ev-function.c
user_ev_function_la_LDFLAGS = -module

lib_LTLIBRARIES += user-ev-predicate.la
user_ev_predicate_la_SOURCES = user-ev-predicate.c
user_ev_predicate_la_LDFLAGS = -module

lib_LTLIBRARIES += user-action.la
user_action_la_SOURCES = user-action.c
user_action_la_LDFLAGS = -module

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = openprs.pc mp-openprs.pc

# These specific rules are here to build the various grammars from
# various files. This could probably be done with cpp... but 20 years
# ago, I thought this was the smartest way...


LEX_INCLUDER = $(top_builddir)/util/lex-includer
AD2C = $(top_srcdir)/util/ad2c
ADLANG2AD = $(top_srcdir)/util/adlang2ad

#
# This rule says that .y files depends of the .y.part files
#

%.y: %.y.part $(LEX_INCLUDER)
	rm -f $*.y
	 $(LEX_INCLUDER) -p.:$(VPATH) < $< > $*.y
	chmod -w $*.y

#
# This rule says that some c and h depend of the yacc they come from.
#

# For debuging the grammer, use:
# $(YACC) $(YFLAGS) --debug --verbose -p oprs_yy -b $* $<

BUILT_SOURCES = oprs-parser.h oprs-server-parser.h ope-parser.h opc-parser.h
AM_YFLAGS = -d -p oprs_yy

# %.tab.c %.tab.h: %.y
# 	$(YACC) -d -p oprs_yy -o $*.tab.c $<

oprs-parser.y: yacc-file.y.part yacc-graph-op.y.part yacc-exp.y.part yacc-text-op.y.part

ope-parser.y: yacc-exp.y.part yacc-text-op.y.part

opc-parser.y:  yacc-file.y.part yacc-graph-op.y.part yacc-exp.y.part yacc-text-op.y.part

# Same for .lex grammars which are built from components
%.l: %.l.part $(LEX_INCLUDER)
	rm -f $*.l
	 $(LEX_INCLUDER) -p.:$(VPATH) < $< > $*.l
	chmod -w $*.l

# I could not rely on the automake flex management,as ylwrap does not properly handle renaming with -P

BUILT_SOURCES += oprs-lex.h oprs-server-lex.h opc-lex.h ope-lex.h

# These two rules are equivalent... it depends which one is requested first... the .h or the .c ...
%-lex.h: %-lex.l
	$(LEX) $(LFLAGS) --header-file=$@ -Poprs_yy -o $(basename $<).c $<

%-lex.c: %-lex.l
	$(LEX) $(LFLAGS) --header-file=$(basename $<).h -Poprs_yy -o $@ $<

ope-lex.l: lex-simple-def.l.part lex-parser-def.l.part lex-macro.l.part lex-graph-op.l.part lex-text-op.l.part lex-meta-exp.l.part lex-exp.l.part

oprs-server-lex.l: lex-simple-def.l.part lex-parser-def.l.part lex-macro.l.part lex-exp.l.part

opc-lex.l: lex-simple-def.l.part lex-parser-def.l.part lex-macro.l.part lex-graph-op.l.part lex-text-op.l.part lex-meta-exp.l.part lex-exp.l.part

oprs-lex.l: lex-simple-def.l.part lex-parser-def.l.part lex-macro.l.part lex-graph-op.l.part lex-text-op.l.part lex-meta-exp.l.part lex-exp.l.part


BUILT_SOURCES += XOprs.ad.h Ope.ad.h

xdefaultdir = $(libdir)/app-defaults
xdefault_DATA = XOprs.ad Ope.ad

#
# This rule says that the .ad.h are obtained with the coresponding .ad
# I add ad2c in case it is not there... to avoid emptying the file.
#
%.ad.h: %.ad $(AD2C)
	$(AD2C) $< > $@

# %-fr.ad: %.adlang $(ADLANG2AD)
# 	rm -f $@
# 	$(ADLANG2AD) fr < $< > $@
# # To make sure we do not modify these...
# 	chmod -w $@

%.ad: %.adlang $(ADLANG2AD)
	rm -f $@
	$(ADLANG2AD) en < $< > $@
# To make sure we do not modify these...
	chmod -w $@


CLEANFILES =  XOprs.ad.h oprs-server-lex.l ope-lex.l ope-parser.y oprs-server-parser.c \
	oprs-parser.y oprs-lex.l opc-parser.c oprs-server-lex.c oprs-parser.h opc-parser.h \
	oprs-lex.c oprs-server-parser.y oprs-server-parser.h ope-parser.h oprs-parser.c \
	Ope.ad.h opc-lex.l opc-parser.y ope-lex.c opc-lex.c ope-parser.c Ope.ad XOprs.ad \
	opc-lex.h  ope-lex.h  oprs-lex.h  oprs-server-lex.h gope-icon.h goprs-icon.h

#BUILT_SOURCES += user-external.c user-external.h user-external_f.h

EXTRA_DIST += default-user-external.c default-user-external.h default-user-external_f.h

if WANT_GTK
%.h: %.png
	gdk-pixbuf-csource --name "my_icon" $< > $@

BUILT_SOURCES += goprs-icon.h gope-icon.h
endif
