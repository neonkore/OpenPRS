#
# This Makefile should be called from where you want the dist-dir to be created...
#
#

.DEFAULT:

VERSION = 1.0b2

OPENPRS-FTP-DIR = dist-dir

DOC-HTML = $(OPENPRS-FTP-DIR)/openprs-$(VERSION)-doc-html.tgz

DOC-PDF = $(OPENPRS-FTP-DIR)/openprs-$(VERSION)-doc.pdf

OPENPRS-REST = $(OPENPRS-FTP-DIR)/openprs-$(VERSION)-rest.tgz

OPENPRS-SRC = $(OPENPRS-FTP-DIR)/openprs-$(VERSION)-src.tgz

README = $(OPENPRS-FTP-DIR)/README
LICENSE = $(OPENPRS-FTP-DIR)/LICENSE
INSTALL = $(OPENPRS-FTP-DIR)/INSTALL
CHANGELOG = $(OPENPRS-FTP-DIR)/ChangeLog

BINARIES = $(OPENPRS-FTP-DIR)/openprs-$(VERSION)-sparc-solaris.tgz \
	   $(OPENPRS-FTP-DIR)/openprs-$(VERSION)-powerpc-darwin.tgz \
	   $(OPENPRS-FTP-DIR)/openprs-$(VERSION)-i386-linux.tgz


all: $(DOC-PDF) $(OPENPRS-REST) $(OPENPRS-SRC) $(README) $(INSTALL) \
	$(LICENSE) $(CHANGELOG) $(BINARIES)

dist: all
	scp $(DOC-PDF) $(OPENPRS-REST) $(OPENPRS-SRC) $(README) \
		$(INSTALL) $(LICENSE) $(CHANGELOG) $(BINARIES) \
		rust:/var/ftp/pub/openrobots/openprs/

$(DOC-PDF): doc/oprs.pdf
	cp $^ $@

$(OPENPRS-REST): app-defaults/* contrib/include/* contrib/src/* data/* include/* pub_src/* util/*
	gnutar zcf $@ app-defaults contrib/include contrib/src data include pub_src util

$(OPENPRS-SRC): src/* src/*/*
	gnutar zcf $@ src

$(README): src/README
	cp $^ $@

$(CHANGELOG): src/ChangeLog
	cp $^ $@

$(LICENSE): src/LICENSE
	cp $^ $@

$(INSTALL): src/dist/INSTALL
	cp $^ $@

$(OPENPRS-FTP-DIR)/openprs-$(VERSION)-%.tgz: %/bin %/lib %/site.make contrib/%/lib
	gnutar zchf $@ $^
