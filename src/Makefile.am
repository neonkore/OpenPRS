AM_CPPFLAGS = -I$(top_srcdir)/c_toolkit 

EXTERNAL_INCLUDE = action_f-pub.h constant-pub.h ev-function_f-pub.h ev-predicate_f-pub.h \
	fact-goal_f-pub.h int-graph_f-pub.h intention_f-pub.h lang.h oprs-sprint-pub.h \
	op-instance_f-pub.h op-structure_f-pub.h lisp-list_f-pub.h macro-pub.h \
	mp-pub.h opaque-pub.h oprs-array_f-pub.h oprs-type-pub.h oprs-sprint_f-pub.h \
	oprs-type_f-pub.h oprs_f-pub.h user-action.h user-ev-function.h \
	user-ev-predicate.h user-external.h user-external_f.h xoprs-main-pub.h oprs-error-pub.h \
	oprs-rerror_f-pub.h user-trace.h user-trace_f.h oprs-print_f-pub.h

pkginclude_HEADERS = $(EXTERNAL_INCLUDE)

#
lib_LTLIBRARIES = libExtMP.la 
noinst_LTLIBRARIES = libMP.la libBasic.la libOprs.la libTOpEOprs.la libGOpEOprs.la \
		libTOprs.la libGOprs.la libKernel.la libTKernel.la libGKernel.la libOpG.la

noinst_LIBRARIES = libVersion.a

LIB_MP_SOURCES = mp-register.c send-message.c lang.c

libVersion_a_SOURCES = version.c
libVersion_a_CPPFLAGS = $(AM_CPPFLAGS) -DCOMPIL_DATE="\"`date`\"" -DHOST="\"`hostname`\""

# this is for external program... do not confuse them with our homebrew malloc.
libExtMP_la_SOURCES = $(LIB_MP_SOURCES)
libExtMP_la_CPPFLAGS =  $(AM_CPPFLAGS) -DEXT_MALLOC

# this is for our own usage
libMP_la_SOURCES = $(LIB_MP_SOURCES)

#
libBasic_la_SOURCES = oprs-socket.c oprs-util.c

libOprs_la_SOURCES = help.c lisp-list.c oprs-ll.c oprs-send-m.c oprs-sprint.c \
		parser-funct.c oprs-array.c oprs-copy.c oprs-dup.c oprs-free.c oprs-print.c \
		oprs-error.c op-compiler.c type.c oprs-pred-func.c

libKernel_la_SOURCES = database.c ev-predicate.c fact-goal.c oprs.c oprs-rerror.c unification.c oprs-profiling.c

libOpG_la_SOURCES = ope-external.c ope-graphic.c xhelp.c oprs-pprint.c ope-report.c

#
BOTH_OPE_OPRS_LIB_SOURCES = oprs-dump.c oprs-load.c

libTOpEOprs_la_SOURCES = $(BOTH_OPE_OPRS_LIB_SOURCES)
libTOpEOprs_la_CPPFLAGS =  $(AM_CPPFLAGS) -DNO_GRAPHIX

libGOpEOprs_la_SOURCES = $(BOTH_OPE_OPRS_LIB_SOURCES)
libGOpEOprs_la_CPPFLAGS =  $(AM_CPPFLAGS) -UNO_GRAPHIX

#
BOTH_OPRS_LIB_SOURCES = op-structure.c oprs-subst.c oprs-type.c conditions.c top-structure.c consult-rop.c relevant-op.c  op-dump-load.c

libTOprs_la_SOURCES = $(BOTH_OPRS_LIB_SOURCES)
libTOprs_la_CPPFLAGS =  $(AM_CPPFLAGS) -DNO_GRAPHIX

libGOprs_la_SOURCES = $(BOTH_OPRS_LIB_SOURCES)
libGOprs_la_CPPFLAGS =  $(AM_CPPFLAGS) -UNO_GRAPHIX

#
BOTH_KERNEL_LIB_SOURCES = action.c activate.c ev-function.c shary-user-ev-function.c intend.c intention.c \
	int-graph.c op-instance.c oprs-init.c tcl.c soak.c oprs-parser.y \
	top-lev.c

libTKernel_la_SOURCES = $(BOTH_KERNEL_LIB_SOURCES)
libTKernel_la_CPPFLAGS =  $(AM_CPPFLAGS) -I$(top_srcdir)/contrib/src -DNO_GRAPHIX

libGKernel_la_SOURCES = $(BOTH_KERNEL_LIB_SOURCES)
libGKernel_la_CPPFLAGS =  $(AM_CPPFLAGS) -I$(top_srcdir)/contrib/src -UNO_GRAPHIX

# This will soon disappear with dl mechanism...
USER_SOURCES = user-action.c user-ev-function.c user-ev-predicate.c user-external.c user-trace.c

CONTRIB_LIB = $(top_builddir)/contrib/src/libpu.la

# Here is the list of program to build and install
bin_PROGRAMS = mp-oprs kill-mp oprs-server oprs xoprs oprs-cat ope opc

mp_oprs_SOURCES = mp.c
mp_oprs_LDADD =  ../c_toolkit/libOPList.la libBasic.la libMP.la libVersion.a

kill_mp_SOURCES = kill-mp.c
kill_mp_LDADD =  ../c_toolkit/libOPList.la libBasic.la libMP.la libVersion.a

oprs_server_SOURCES = oprs-server-parser.y oprs-server-lex.l opaque-ext.c oprs-client.c \
		oprs-server-main.c oprs-server-opaque.c
oprs_server_LDADD = ../c_toolkit/libOPList.la libBasic.la libTOprs.la libOprs.la libMP.la \
		 libVersion.a $(READLINE_LIBS)

oprs_SOURCES = oprs-lex.l op-x-opaque.c oprs-main.c top-lev-loop.c $(USER_SOURCES)
oprs_LDADD = libTKernel.la libKernel.la libBasic.la libTOprs.la  libOprs.la libTOpEOprs.la \
		 libMP.la libVersion.a ../c_toolkit/libOPList.la $(CONTRIB_LIB)

xoprs_SOURCES =  oprs-lex.l xoprs-call.c xoprs-textwin.c $(USER_SOURCES) \
	xoprs-dialog.c xoprs-filesel.c xoprs-intention.c \
	xoprs-main.c xoprs-menu.c xt-util.c xtop-lev-loop.c \
	xoprs-op-graphic.c xoprs-rop.c
xoprs_LDADD =   libGKernel.la libKernel.la libBasic.la libGOprs.la libOprs.la libGOpEOprs.la \
		libOpG.la libMP.la libVersion.a ../c_toolkit/libOPList.la $(CONTRIB_LIB) $(MOTIF_LIBS) $(X_LIBS)

oprs_cat_SOURCES =  oprs-cat.c
oprs_cat_LDADD = ../c_toolkit/libOPList.la

ope_SOURCES = ope-lex.l ope-parser.y ope-bboard.c ope-edit.c ope-filesel.c ope-op-opf.c ope-main.c \
	ope-menu.c ope-print.c ope-pxmw.c \
	ope-save.c ope-syntax.c \
	ope-vsblf.c xt-util.c ope-rop.c ope-op-str.c ope-opaque.c
ope_LDADD = libBasic.la libGOprs.la libOprs.la libGOpEOprs.la \
		libOpG.la libMP.la libVersion.a ../c_toolkit/libOPList.la $(MOTIF_LIBS) $(X_LIBS)

opc_SOURCES = opc-lex.l opc-parser.y opc-main.c ope-opaque.c
opc_LDADD = libBasic.la libGOprs.la libOprs.la libGOpEOprs.la \
	libOpG.la libMP.la libVersion.a ../c_toolkit/libOPList.la $(MOTIF_LIBS) $(X_LIBS)


pkgconfigdir = $(libdir)/pkgconfig 
pkgconfig_DATA = openprs.pc mp-openprs.pc

# These specific rules are here to build the various grammars from
# various files. This could probably be done with cpp... but 20 years
# ago, I thought this was the smartest way...


LEX_INCLUDER = $(top_builddir)/util/lex-includer
AD2C = $(top_builddir)/util/ad2c
ADLANG2AD = $(top_builddir)/util/adlang2ad

#
# This rule says that .y files depends of the .y.part files
#

%.y: %.y.part $(LEX_INCLUDER)
	rm -f $*.y
	 $(LEX_INCLUDER) -p.:$(VPATH) < $< > $*.y
	chmod -w  $*.y

#
# This rule says that some c and h depend of the yacc they come from.
#

# For debuging the grammer, use:
# $(YACC) $(YFLAGS) --debug --verbose -p oprs_yy -b $* $<

BUILT_SOURCES = oprs-parser.h oprs-server-parser.h ope-parser.h opc-parser.h
AM_YFLAGS = -d -p oprs_yy

# %.tab.c %.tab.h: %.y
# 	$(YACC) -d -p oprs_yy -o $*.tab.c $<

oprs-parser.y ope-parser.y opc-parser.y oprs-server-parser.y: yacc-file.y.part yacc-graph-op.y.part yacc-exp.y.part yacc-text-op.y.part

# Same for .lex grammars which are built from components
%.l: %.l.part  $(LEX_INCLUDER)
	rm -f $*.l
	 $(LEX_INCLUDER) -p.:$(VPATH) < $< > $*.l
	chmod -w  $*.l

# I could not rely on the automake flex management,as ylwrap  does not properly handle renaming with -P
%.c: %.l
	$(LEX) $(LFLAGS) -Poprs_yy -o $@  $<


#
# This rule says that the .ad.h are obtained with the coresponding .ad
# I add util/ad2c in case it is not there... to avoid emptying the file.
#
%.ad.h: %.ad $(AD2C)
	$(AD2C) $< > $@

%-fr.ad: %.adlang $(ADLANG2AD)
	rm -f $@
	$(ADLANG2AD) fr < $< > $@
# To make sure we do not modify these...
	chmod -w $@

%-en.ad: %.adlang $(ADLANG2AD)
	rm -f $@
	$(ADLANG2AD) en < $< > $@
# To make sure we do not modify these...
	chmod -w $@

