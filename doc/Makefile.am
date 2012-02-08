#Nothing for now


OP_TEX = bar.tex example-cons.tex fact-meta-if-par.tex fact-meta.tex \
	fib-par.tex fib.tex meta-intended-goal.tex new-default.tex \
	new-meta-ops.tex truck-demo-plus.tex truck-demo.tex wait.tex \
	semaphore.tex

OP_EDITOR = $(top_builddir)/src/ope

INDEX = oprs.cpnd oprs.fnnd oprs.epnd oprs.efnd oprs.vrnd oprs.pgnd 

doc_DATA = oprs.pdf

oprs.pdf: oprs.tex oprs-macros.tex oprs.bib $(OP_TEX) $(FIGURES) appendices.tex
	$(LATEX) $<
	$(MAKE) index
	$(BIBTEX) oprs
	$(LATEX) $<
	$(LATEX) $<

index: $(INDEX)

%.tex: ../data/%.opf
	$(OP_EDITOR) -t $< $@

%.tex: ../data/test/%.opf
	$(OP_EDITOR) -t $< $@

%nd: %dx
	 $(MAKEINDEX) $< -o  $@
