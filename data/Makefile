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
###############################################################################

#
# the file site.make is a link to a file containing machine/site dependencies.
#

#include ../site.make

include ../path.make

DESTDIR = $(srctop)/..

srcdir = $(srctop)/data

VPATH = $(srcdir)

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
	database-ep.inc database-ff.inc \
	fact-meta.inc fib.inc unification.inc \
	foo.inc fact-meta-if-par.inc fib-par.inc ipp.inc\
	fact-text.inc new-default.inc semaphore.inc mini-default.inc

GGRAPH_FILES = 

SYM_FILES = meta-intended-goal.sym new-default.sym semaphore.sym \
	mini-default.sym common-meta-processes.sym opc-declaration.sym

DB_FILES = include.db

OPF_TEST_FILES = bind.opf effects.opf empty.opf \
	fail.opf  meta-fact-goal.opf\
	maint.opf all.opf array.opf \
	boundp.opf car-cdr.opf command.opf \
	copy-env.opf current.opf error.opf foo.opf \
	goal-old.opf goal.opf intention.opf mtime.opf \
	mult-var.opf prop.opf read.opf test.opf wait.opf \
	ff-val.opf cons-term.opf del-ll.opf special.opf \
	critical.opf text-opf-torture.opf append.opf \
	semaphore.opf

INC_TEST_FILES = empty.inc database.inc all.inc \
	array.inc bind.inc boundp.inc car-cdr.inc \
	copy-env.inc current.inc failed.inc goal.inc \
	intention.inc maint.inc read.inc \
	wait.inc ff-val.inc cons-term.inc \
	del-ll.inc special.inc critical.inc \
	text-opf-torture.inc semaphore.inc meta-fact-goal.inc ipp-test.inc

DB_TEST_FILES = bind.db maint.db ipp-test.db

SGRAPH_TEST_FILES = fact.sgraph test.sgraph wait.sgraph

INSTALL_DATA_FILES = $(SYM_FILES) $(INC_FILES) $(OPF_FILES) $(DB_FILES)

INSTALL_TEST_FILES = $(INC_TEST_FILES) $(OPF_TEST_FILES) $(DB_TEST_FILES) $(SGRAPH_TEST_FILES)

INSTALL = $(srctop)/util/install.sh
INSTALL-IF-CHANGE = $(srctop)/util/install-if-change.sh

all:

install: $(INSTALL_DATA_FILES) 
	for i in $(INSTALL_DATA_FILES) ; do\
		$(INSTALL-IF-CHANGE) -c -m 644 $$i ${DESTDIR}/data/$$i;\
	done
	for i in $(INSTALL_TEST_FILES) ; do\
		$(INSTALL-IF-CHANGE) -c -m 644 test/$$i ${DESTDIR}/data/test/$$i;\
	done

