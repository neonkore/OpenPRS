#
# This Makefile should be called from ../..
#
#

.DEFAULT:

OPENPRS-FTP-DIR = dist-dir

DOC-HTML = $(OPENPRS-FTP-DIR)/openprs-1.0b-doc-html.tgz

DOC-PDF = $(OPENPRS-FTP-DIR)/openprs-1.0b-doc-pdf.tgz

OPENPRS-REST = $(OPENPRS-FTP-DIR)/openprs-1.0b-rest.tgz

OPENPRS-SRC = $(OPENPRS-FTP-DIR)/openprs-1.0b-src.tgz

README = $(OPENPRS-FTP-DIR)/README
LICENSE = $(OPENPRS-FTP-DIR)/LICENSE
INSTALL = $(OPENPRS-FTP-DIR)/INSTALL

all: $(DOC-PDF) $(OPENPRS-REST) $(OPENPRS-SRC) $(README) $(INSTALL) $(LICENSE)\
	 $(OPENPRS-FTP-DIR)/openprs-1.0b-sparc-solaris.tgz \
	 $(OPENPRS-FTP-DIR)/openprs-1.0b-i386-linux.tgz

dist: all
	scp $(DOC-PDF) $(OPENPRS-REST) $(OPENPRS-SRC) $(README) $(INSTALL) $(LICENSE) $(OPENPRS-FTP-DIR)/openprs-1.0b-sparc-solaris.tgz $(OPENPRS-FTP-DIR)/openprs-1.0b-i386-linux.tgz rust:/var/ftp/pub/openrobots/openprs/

$(DOC-PDF): doc/oprs.pdf
	gnutar zcf $@ $^

$(OPENPRS-REST): app-defaults/* contrib/include/* contrib/src/* data/* include/* pub_src/* util/*
	gnutar zcf $@ app-defaults contrib/include contrib/src data include pub_src util

$(OPENPRS-SRC): src/* src/*/*
	gnutar zcf $@ src

$(README): src/dist/README
	cp $^ $@

$(LICENSE): src/dist/LICENSE
	cp $^ $@

$(INSTALL): src/dist/INSTALL
	cp $^ $@

$(OPENPRS-FTP-DIR)/openprs-1.0b-%.tgz: %/bin %/lib %/site.make contrib/%/lib
	gnutar zchf $@ $^

$(OPENPRS-FTP-DIR)/openprs-1.0b-%-vxworks.tgz: %-vxworks/lib %-vxworks/site.make contrib/%-vxworks/lib
	gnutar zchf $@ $^
