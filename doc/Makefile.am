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


OP_TEX = bar.tex example-cons.tex fact-meta-if-par.tex fact-meta.tex \
	fib-par.tex fib.tex meta-intended-goal.tex new-default.tex \
	new-meta-ops.tex truck-demo-plus.tex truck-demo.tex wait.tex \
	semaphore.tex

OP_EDITOR = $(top_builddir)/src/ope

INDEX = oprs.cpnd oprs.fnnd oprs.epnd oprs.efnd oprs.vrnd oprs.pgnd 

doc_DATA = oprs.pdf

export BIBINPUTS = .:$(top_srcdir)/doc:
export TEXINPUTS = .:$(top_srcdir)/doc:$(top_srcdir)/doc/fig:

oprs.pdf: oprs.tex oprs-kernel.tex oprs-macros.tex oprs.bib $(OP_TEX) appendices.tex
	$(LATEX) $<
	$(MAKE) index
	$(BIBTEX) oprs
	$(LATEX) $<
	$(LATEX) $<
	$(LATEX) $<

index: $(INDEX)

%.tex: $(top_srcdir)/data/%.opf
	$(OP_EDITOR) -t $< $@

%.tex: $(top_srcdir)/data/test/%.opf
	$(OP_EDITOR) -t $< $@

%.tex: $(top_srcdir)/demo/truck-demo/oprs/data/%.opf
	$(OP_EDITOR) -t $< $@

%nd: %dx
	 $(MAKEINDEX) $< -o  $@

EXTRA_DIST = oprs.tex oprs-macros.tex oprs.bib  appendices.tex fig truck-demo-plus.tex \
	truck-demo.tex biblio.tex oprs-yacc.tex oprs-lex.tex copyright.tex license.tex \
	overview.tex oprs-kernel.tex oprs-server.tex mp.tex xoprs-kernel.tex opc.tex \
	ope.tex using-oprs.tex

CLEANFILES =  oprs.cpdx oprs.aux fact-meta.tex oprs.pgdx oprs.out license.aux oprs.pgnd \
	oprs.blg oprs.lot fib.tex mp.aux oprs.epnd oprs.fndx oprs.vrdx appendices.aux \
	oprs.epdx oprs.ilg biblio.aux oprs.efdx copyright.aux oprs.vrnd ope.aux using-oprs.aux \
	oprs.pdf oprs.fnnd wait.tex bar.tex oprs.log fib-par.tex semaphore.tex overview.aux \
	meta-intended-goal.tex oprs-kernel.aux opc.aux oprs.bbl oprs.toc oprs.efnd oprs.lof \
	oprs-server.aux oprs.cpnd new-default.tex fact-meta-if-par.tex example-cons.tex \
	xoprs-kernel.aux new-meta-ops.tex
