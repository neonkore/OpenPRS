# 

SUBDIRS = test

LEAK_FILES = leak.inc leak1.inc leak10.inc leak2.inc leak3.inc leak4.inc leak5.inc leak6.inc leak7.inc \
		leak8.inc leak9.inc


OPF_FILES = example-cons.opf meta-intended-goal.opf example-list.opf \
	bar.opf new-default.opf meta-selector.opf \
	new-meta-ops.opf boo.opf com.opf \
	conway.opf common-meta-processes.opf \
	fact-meta.opf fib.opf foo.opf \
	fact-meta-if-par.opf fib-par.opf ipp.opf\
	fact-text.opf fib-text.opf semaphore.opf mini-default.opf

INC_FILES = example-list.inc include.inc bar.inc boo.inc database-cwp.inc \
	database-ep.inc database-ff.inc meta-intended-goal.inc\
	fact-meta.inc fib.inc unification.inc \
	foo.inc fact-meta-if-par.inc fib-par.inc ipp.inc\
	fact-text.inc new-default.inc semaphore.inc mini-default.inc

GGRAPH_FILES = 

SYM_FILES = meta-intended-goal.sym new-default.sym semaphore.sym \
	mini-default.sym common-meta-processes.sym opc-declaration.sym

DB_FILES = include.db

pkgdatadatadir = $(pkgdatadir)/data
dist_pkgdatadata_DATA = $(SYM_FILES) $(INC_FILES) $(OPF_FILES) $(DB_FILES)
