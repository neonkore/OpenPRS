########################### -*- Mode: Makefile -*- #########################
## Makefile -- 
## 
## Copyright (C) 1992-2001 Francois Felix Ingrand
## 
## Author          : Francois Felix Ingrand
## Created On      : Wed Apr  1 15:00:21 1992
## Last Modified By: Francois Felix Ingrand
## Last Modified On: Tue Oct 21 13:50:03 2003
## Update Count    : 271
## Status          : OK
## 
## $Id$
## 
###############################################################################
# Copyright (c) 1991-2001 Francois Felix Ingrand.
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

.SUFFIXES: .Z

.PHONY:	all index

srcdir=../../../src/doc

VPATH = .:$(srcdir)

include ../path.make

include ../site.make

FIGURES = $(srcdir)/fig/*.ps

INDEX = oprs.cpnd oprs.fnnd oprs.epnd oprs.efnd oprs.vrnd oprs.pgnd 

INDEX_RAW = $(INDEX:%nd=%dx)

OP_TEX = bar.tex example-cons.tex fact-meta-if-par.tex fact-meta.tex \
	fib-par.tex fib.tex meta-intended-goal.tex new-default.tex \
	new-meta-ops.tex truck-demo-plus.tex truck-demo.tex wait.tex \
	semaphore.tex

OP_TEXT = fact-text.txt-opf fib-text.txt-opf

OP_EDITOR = ../ope

X_OPRS = ../xoprs


all: oprs.pdf

#oprs.dvi: oprs-macros.tex $(OP_TEX) $(OP_TEXT) $(FIGURES) $(INDEX)

oprs.dvi: oprs.tex oprs-macros.tex oprs.bib $(OP_TEX) $(OP_TEXT) $(FIGURES)
	latex $<
	$(MAKE) index
	bibtex oprs
	latex $<
	latex $<

oprs.pdf: oprs.tex oprs-macros.tex oprs.bib $(OP_TEX) $(OP_TEXT) $(FIGURES)
	pdflatex $<
	$(MAKE) index
	bibtex oprs
	pdflatex $<
	pdflatex $<

index: $(INDEX)

oprs: $(OP_TEX) $(OP_TEXT)

%.ps: %.dvi
	dvips $< -o $@

%nd: %dx
	 makeindex $< -o  $@

clean:
	$(RM) -rf *.aux oprs.??dx oprs.??nd oprs.??.ilg oprs $(OP_TEXT)\
		oprs.lof oprs.lot oprs.toc oprs.log oprs.bbl oprs.blg oprs.ps oprs.dvi \
		oprs $(OP_TEX)


install: oprs.pdf
	$(srcdir)/../util/install.sh -c -m 644 $< ${DESTDIR}/doc/$@;\

# Cut here for updating the Makefile.src file
# (DO NOT TOUCH THE PREVIOUS LINE)
# The next entry depends on it...

Makefile.src: Makefile.dev
	sed -e '/^# Cut here for updating the Makefile.src file/,$$d'  <Makefile.dev >Makefile.src

ifdef US_LETTER
paper-size.tex:  us-letter-size.tex
else
paper-size.tex:  a4-size.tex
endif
	$(RM) -f $@
	ln -s $< $@

a4:
	$(RM) -f paper-size.tex oprs.dvi
	make oprs.ps
	ln oprs.ps oprs.ps.a4

us-letter:
	$(RM) -f paper-size.tex oprs.dvi
	make oprs.ps US_LETTER=1	
	ln oprs.ps oprs.ps.us

%.tex: ../data/%.opf $(OP_EDITOR)
	$(OP_EDITOR) -iconic -t $< $@

%.txt-opf: ../data/%.opf
	cp $< $@

%.tex: ../data/test/%.opf
	$(OP_EDITOR) -iconic -t $< $@

truck-demo.tex: ../demo/truck-demo/oprs/data/truck-demo.opf
	$(OP_EDITOR) -iconic -t $< $@

truck-demo-plus.tex: ../demo/truck-demo/oprs/data/truck-demo-plus.opf
	$(OP_EDITOR) -iconic -t $< $@

$(OP_EDITOR):
	(cd .. ; $(MAKE) ope)

$(X_OPRS):
	(cd .. ; $(MAKE) xoprs)

