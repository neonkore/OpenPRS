AM_CPPFLAGS = -I$(top_srcdir)/c_toolkit 

INC = action.h activate.h config.h constant-pub.h constant.h database.h \
	ev-function.h ev-predicate.h fact-goal.h int-graph.h \
	intend.h intention.h op-compiler.h op-instance.h op-structure.h \
	ope-global.h ope-graphic.h lisp-list.h macro-pub.h macro.h mp.h \
	mp-pub.h mp-register.h opaque-ext.h opaque-pub.h opaque.h oprs-sprint-pub.h \
	parser-funct.h oprs-client.h oprs-main.h oprs-print.h oprs-server.h \
	oprs-profiling.h oprs-type-pub.h oprs-type.h oprs.h relevant-op.h soak.h \
	top-lev.h unification.h user-action.h user-trace.h user-ev-function.h \
	user-ev-predicate.h version.h xoprs.h xhelp.h \
	xoprs-intention.h xoprs-main-pub.h xoprs-main.h xoprs.h xoprs-textwin.h ope-syntax.h \
	op-default.h oprs-sprint.h oprs-error.h oprs-rerror.h oprs-error-pub.h \
	conditions.h lang.h oprs-dump.h opc.h type.h oprs-pred-func.h tcl.h yy-funct.c

FINC = action_f.h activate_f.h database_f.h ev-function_f.h oprs-pred-func_f.h \
	ev-predicate_f.h fact-goal_f-pub.h fact-goal_f.h \
	help_f.h int-graph_f-pub.h int-graph_f.h intend_f.h intention_f-pub.h \
	intention_f.h op-compiler_f.h op-instance_f-pub.h op-instance_f.h \
	op-structure_f.h ope-bboard_f.h ope-edit_f.h ope-external_f.h \
	ope-filesel_f.h ope-graphic_f.h ope-op-opf_f.h ope-menu_f.h \
	ope-print_f.h ope-pxmw_f.h ope-save_f.h ope-syntax_f.h ope-vsblf_f.h \
	lisp-list_f-pub.h lisp-list_f.h mp-register_f.h other_proto.h \
	parser-funct_f.h oprs-array_f.h oprs-client_f.h oprs-print_f.h \
	oprs-send-m_f.h oprs-socket_f.h oprs-profiling_f.h oprs-type_f-pub.h \
	oprs-type_f.h oprs-util_f.h oprs_f-pub.h oprs_f.h relevant-op_f.h \
	send-message_f.h soak_f.h socket_f.h xt-util_f.h\
	top-lev_f.h unification_f.h usage_f.h xhelp_f.h \
	xoprs-intention_f.h xoprs-menu_f.h xoprs-textwin_f.h ope-rop_f.h ope-report_f.h \
	oprs-pprint_f.h tcl_f.h oprs-init_f.h oprs-sprint_f.h oprs-error_f.h \
	oprs-rerror_f-pub.h oprs-sprint_f-pub.h conditions_f.h oprs-rerror_f.h \
	oprs-dump_f.h oprs-print_f-pub.h type_f.h top-structure_f.h tcl_f.h

YACC_PART = opc-parser.y.part oprs-parser.y.part opt-parser.y.part yacc-file.y.part \
	yacc-text-op.y.part ope-parser.y.part oprs-server-parser.y.part \
	yacc-exp.y.part yacc-graph-op.y.part

LEX_PART = lex-exp.l.part lex-macro.l.part lex-parser-def.l.part lex-text-op.l.part \
	ope-lex.l.part oprs-server-lex.l.part lex-graph-op.l.part lex-meta-exp.l.part \
	lex-simple-def.l.part opc-lex.l.part oprs-lex.l.part

DIST_ANNEX_FILES = ope-icon.bit xoprs-icon.bit

XDEFAULT = XOprs.adlang Ope.adlang

EXTRA_DIST = $(INC) $(FINC) $(YACC_PART) $(LEX_PART) $(XDEFAULT) $(DIST_ANNEX_FILES)

EXTERNAL_INCLUDE = action_f-pub.h constant-pub.h ev-function_f-pub.h ev-predicate_f-pub.h \
	fact-goal_f-pub.h int-graph_f-pub.h intention_f-pub.h lang.h oprs-sprint-pub.h \
	op-instance_f-pub.h op-structure_f-pub.h lisp-list_f-pub.h macro-pub.h \
	mp-pub.h opaque-pub.h oprs-array_f-pub.h oprs-type-pub.h oprs-sprint_f-pub.h \
	oprs-type_f-pub.h oprs_f-pub.h user-action.h user-ev-function.h \
	user-ev-predicate.h xoprs-main-pub.h oprs-error-pub.h \
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
libExtMP_la_CPPFLAGS = $(AM_CPPFLAGS) -DEXT_MALLOC

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
libTOpEOprs_la_CPPFLAGS = $(AM_CPPFLAGS) -DNO_GRAPHIX

libGOpEOprs_la_SOURCES = $(BOTH_OPE_OPRS_LIB_SOURCES)
libGOpEOprs_la_CPPFLAGS = $(AM_CPPFLAGS) -UNO_GRAPHIX

#
BOTH_OPRS_LIB_SOURCES = op-structure.c oprs-subst.c oprs-type.c conditions.c top-structure.c consult-rop.c relevant-op.c op-dump-load.c

libTOprs_la_SOURCES = $(BOTH_OPRS_LIB_SOURCES)
libTOprs_la_CPPFLAGS = $(AM_CPPFLAGS) -DNO_GRAPHIX

libGOprs_la_SOURCES = $(BOTH_OPRS_LIB_SOURCES)
libGOprs_la_CPPFLAGS = $(AM_CPPFLAGS) -UNO_GRAPHIX

#
BOTH_KERNEL_LIB_SOURCES = action.c activate.c ev-function.c intend.c intention.c \
	int-graph.c op-instance.c oprs-init.c tcl.c soak.c oprs-parser.y \
	top-lev.c default-hook.c

libTKernel_la_SOURCES = $(BOTH_KERNEL_LIB_SOURCES)
libTKernel_la_CPPFLAGS = $(AM_CPPFLAGS) -I$(top_srcdir)/contrib/src -DNO_GRAPHIX

libGKernel_la_SOURCES = $(BOTH_KERNEL_LIB_SOURCES)
libGKernel_la_CPPFLAGS = $(AM_CPPFLAGS) -I$(top_srcdir)/contrib/src -UNO_GRAPHIX

# This will soon disappear with dl mechanism...
USER_MODULES_EXAMPLE = user-action.c user-ev-function.c user-ev-predicate.c

CONTRIB_LIB = $(top_builddir)/contrib/src/libpu.la


INCLUDES = @INCLTDL@

# Here is the list of program to build and install
bin_PROGRAMS = mp-oprs kill-mp oprs-server oprs xoprs oprs-cat ope opc

mp_oprs_SOURCES = mp.c
mp_oprs_LDADD = ../c_toolkit/libOPList.la libBasic.la libMP.la libVersion.a

kill_mp_SOURCES = kill-mp.c
kill_mp_LDADD = ../c_toolkit/libOPList.la libBasic.la libMP.la libVersion.a

oprs_server_SOURCES = oprs-server-parser.y oprs-server-lex.l opaque-ext.c oprs-client.c \
		oprs-server-main.c oprs-server-opaque.c
oprs_server_LDADD = ../c_toolkit/libOPList.la libBasic.la libTOprs.la libOprs.la libMP.la \
		 libVersion.a $(READLINE_LIBS)

oprs_SOURCES = oprs-lex.l op-x-opaque.c oprs-main.c top-lev-loop.c user-trace.c
oprs_LDADD = libTKernel.la libKernel.la libBasic.la libTOprs.la libOprs.la libTOpEOprs.la \
		 libMP.la libVersion.a ../c_toolkit/libOPList.la $(CONTRIB_LIB)\
		 @LIBLTDL@ @LIBADD_DL@

xoprs_SOURCES = oprs-lex.l xoprs-call.c xoprs-textwin.c \
	xoprs-dialog.c xoprs-filesel.c xoprs-intention.c \
	xoprs-main.c xoprs-menu.c xt-util.c xtop-lev-loop.c \
	xoprs-op-graphic.c xoprs-rop.c user-trace.c
xoprs_LDADD = libGKernel.la libKernel.la libBasic.la libGOprs.la libOprs.la libGOpEOprs.la \
	libOpG.la libMP.la libVersion.a ../c_toolkit/libOPList.la $(CONTRIB_LIB) \
	@MOTIF_LIBS@ @X_LIBS@ @LIBLTDL@ @LIBADD_DL@

oprs_cat_SOURCES = oprs-cat.c
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


# This is what replace relocatable... dynamic loading of evaluable function, predicates and actions.

lib_LTLIBRARIES += user-ev-function-example.la
user_ev_function_example_la_SOURCES = user-ev-function.c
user_ev_function_example_la_LDFLAGS = -module

lib_LTLIBRARIES += user-ev-predicate-example.la
user_ev_predicate_example_la_SOURCES = user-ev-predicate.c
user_ev_predicate_example_la_LDFLAGS = -module

lib_LTLIBRARIES += user-action-example.la
user_action_example_la_SOURCES = user-action.c
user_action_example_la_LDFLAGS = -module

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

# this could be smarter about which one depends of which...
oprs-parser.y: oprs-parser.y.part yacc-file.y.part yacc-graph-op.y.part yacc-exp.y.part yacc-text-op.y.part

ope-parser.y: ope-parser.y.part yacc-file.y.part yacc-graph-op.y.part yacc-exp.y.part yacc-text-op.y.part

opc-parser.y: opc-parser.y.part yacc-file.y.part yacc-graph-op.y.part yacc-exp.y.part yacc-text-op.y.part

oprs-server-parser.y: oprs-server-parser.y.part yacc-file.y.part yacc-graph-op.y.part yacc-exp.y.part yacc-text-op.y.part

# Same for .lex grammars which are built from components
%.l: %.l.part $(LEX_INCLUDER)
	rm -f $*.l
	 $(LEX_INCLUDER) -p.:$(VPATH) < $< > $*.l
	chmod -w $*.l

# I could not rely on the automake flex management,as ylwrap does not properly handle renaming with -P
%.c: %.l
	$(LEX) $(LFLAGS) -Poprs_yy -o $@ $<

BUILT_SOURCES += XOprs.ad.h Ope.ad.h

xdefaultdir = $(libdir)/app-defaults
xdefault_DATA = XOprs.ad Ope.ad

#
# This rule says that the .ad.h are obtained with the coresponding .ad
# I add util/ad2c in case it is not there... to avoid emptying the file.
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


CLEANFILES =  XOprs.ad.h oprs-server-lex.l ope-lex.l ope-parser.y oprs-server-parser.c oprs-parser.y oprs-lex.l opc-parser.c oprs-server-lex.c oprs-parser.h opc-parser.h oprs-lex.c oprs-server-parser.y oprs-server-parser.h ope-parser.h oprs-parser.c Ope.ad.h opc-lex.l opc-parser.y ope-lex.c opc-lex.c ope-parser.c Ope.ad XOprs.ad


#BUILT_SOURCES += user-external.c user-external.h user-external_f.h

EXTRA_DIST += default-user-external.c default-user-external.h default-user-external_f.h
