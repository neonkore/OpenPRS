OPENPRS-FTP-DIR = /usr/local/ftp/pub/ria/felix/openprs

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

$(DOC-PDF): doc/oprs.pdf
	gnutar zcf $@ $^

$(OPENPRS-REST): app-defaults contrib/include contrib/src data include pub_src util
	gnutar zcf $@ $^

$(OPENPRS-SRC): src
	gnutar zcf $@ $^

$(README): src/dist/README
	cp $^ $@

$(LICENSE): src/dist/LICENSE
	cp $^ $@

$(INSTALL): src/dist/INSTALL
	cp $^ $@

$(OPENPRS-FTP-DIR)/openprs-1.0b-%.tgz: %/bin %/lib %/site.make contrib/%/lib
	gnutar zvchf $@ $^

$(OPENPRS-FTP-DIR)/openprs-1.0b-%-vxworks.tgz: %-vxworks/lib %-vxworks/site.make contrib/%-vxworks/lib
	gnutar zvchf $@ $^
