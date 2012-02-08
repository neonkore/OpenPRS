AM_CPPFLAGS = -I$(top_srcdir)/c_toolkit 

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
	int-graph.c op-instance.c oprs-init.c tcl.c soak.c oprs-parser.tab.c \
	oprs-parser.lex.c top-lev.c

libTKernel_la_SOURCES = $(BOTH_KERNEL_LIB_SOURCES)
libTKernel_la_CPPFLAGS =  $(AM_CPPFLAGS) -I$(top_srcdir)/contrib/src -DNO_GRAPHIX

libGKernel_la_SOURCES = $(BOTH_KERNEL_LIB_SOURCES)
libGKernel_la_CPPFLAGS =  $(AM_CPPFLAGS) -I$(top_srcdir)/contrib/src -UNO_GRAPHIX

# This will soon disappear with dl mechanism...
USER_SOURCES = user-action.c user-ev-function.c user-ev-predicate.c user-external.c user-trace.c

CONTRIB_LIB = $(top_srcdir)/contrib/src/libpu.la

# Here is the list of program to build and install
bin_PROGRAMS = mp-oprs kill-mp oprs-server oprs xoprs oprs-cat ope opc

mp_oprs_SOURCES = mp.c
mp_oprs_LDADD =  ../c_toolkit/libOPList.la libBasic.la libMP.la libVersion.a

kill_mp_SOURCES = kill-mp.c
kill_mp_LDADD =  ../c_toolkit/libOPList.la libBasic.la libMP.la libVersion.a

oprs_server_SOURCES = opaque-ext.c oprs-client.c oprs-server-main.c oprs-server-opaque.c \
	oprs-server-parser.tab.c oprs-server-parser.lex.c
oprs_server_LDADD = ../c_toolkit/libOPList.la libBasic.la libTOprs.la libOprs.la libMP.la \
		 libVersion.a $(READLINE_LIBS)

oprs_SOURCES = op-x-opaque.c oprs-main.c top-lev-loop.c $(USER_SOURCES)
oprs_LDADD = libTKernel.la libKernel.la libBasic.la libTOprs.la  libOprs.la libTOpEOprs.la \
		 libMP.la libVersion.a ../c_toolkit/libOPList.la $(CONTRIB_LIB)

xoprs_SOURCES =  xoprs-call.c xoprs-textwin.c $(USER_SOURCES) \
	xoprs-dialog.c xoprs-filesel.c xoprs-intention.c \
	xoprs-main.c xoprs-menu.c xt-util.c xtop-lev-loop.c \
	xoprs-op-graphic.c xoprs-rop.c
xoprs_LDADD =   libGKernel.la libKernel.la libBasic.la libGOprs.la libOprs.la libGOpEOprs.la \
		libOpG.la libMP.la libVersion.a ../c_toolkit/libOPList.la $(CONTRIB_LIB) $(MOTIF_LIBS) $(X_LIBS)

oprs_cat_SOURCES =  oprs-cat.c
oprs_cat_LDADD = ../c_toolkit/libOPList.la

ope_SOURCES = ope-bboard.c ope-edit.c ope-filesel.c ope-op-opf.c ope-main.c \
	ope-menu.c ope-parser.tab.c ope-parser.lex.c ope-print.c ope-pxmw.c \
	ope-save.c ope-syntax.c \
	ope-vsblf.c xt-util.c ope-rop.c ope-op-str.c ope-opaque.c
ope_LDADD = libBasic.la libGOprs.la libOprs.la libGOpEOprs.la \
		libOpG.la libMP.la libVersion.a ../c_toolkit/libOPList.la $(MOTIF_LIBS) $(X_LIBS)

opc_SOURCES = opc-main.c opc-parser.tab.c opc-parser.lex.c ope-opaque.c
opc_LDADD = libBasic.la libGOprs.la libOprs.la libGOpEOprs.la \
	libOpG.la libMP.la libVersion.a ../c_toolkit/libOPList.la $(MOTIF_LIBS) $(X_LIBS)

# These specific rules are here to build the various grammars from
# various files. This could probably be done with cpp... but 20 years
# ago, I thought this was the smartest way...


LEX_INCLUDER = $(top_srcdir)/util/lex-includer
AD2C = $(top_srcdir)/util/ad2c
ADLANG2AD = $(top_srcdir)/util/adlang2ad

#
# This rule says that .yy files depends of the .y files
#

%.yy: %.y $(LEX_INCLUDER)
	rm -f $*.yy
	 $(LEX_INCLUDER) -p.:$(VPATH) < $< > $*.yy
	chmod -w  $*.yy

#
# This rule says that some c and h depend of the yacc they come from.
#

# For debuging the grammer, use:
# $(YACC) $(YFLAGS) --debug --verbose -p oprs_yy -b $* $<

%.tab.c %.tab.h: %.yy
	$(YACC) -d -p oprs_yy -o $*.tab.c $<

oprs-parser.yy ope-parser.yy opc-parser.yy: yacc-file.y yacc-graph-op.y
oprs-parser.yy ope-parser.yy opc-parser.yy: yacc-exp.y yacc-text-op.y

# Same for .lex grammars which are built from components
%.ll: %.l  $(LEX_INCLUDER)
	rm -f $*.ll
	 $(LEX_INCLUDER) -p.:$(VPATH) < $< > $*.ll
	chmod -w  $*.ll

%.lex.c: %.ll
	$(LEX) $(LEXFLAGS) -L -Poprs_yy -t $< > $@


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

