if RELACS_DOCBOOK_COND

## ------------------------------- ##
## Rules specific for HTML output. ##
## ------------------------------- ##

if RELACS_DOCBOOK_HTML_COND

DOCBOOK_CLEAN_HTML = html

docbook-html: html/index.html

html/index.html: $(DOCBOOK_BASE).xml $(DOCBOOK_BASE).xsl $(DOCBOOK_FILES)
	cd $(srcdir); \
	xmlto -o $(abs_builddir)/html -m $(DOCBOOK_BASE).xsl --skip-validation html $(DOCBOOK_BASE).xml; \
	cp $(DOCBOOK_HTML_EXTRA) $(abs_builddir)/html/ ; \
	for p in $(abs_builddir)/html/*.html; do sed -e 's|^.*</pre|</pre|; s|src="images/|src="|; s|href="relacsapi/|href="../api/classrelacs_1_1|;' $$p > tmp.html; mv tmp.html $$p; done || true; \
	rm -f $(abs_builddir)/html/$(DOCBOOK_BASE).fls

docbook-install-html:
	test -z "$(htmldir)" || $(MKDIR_P) "$(DESTDIR)$(htmldir)$(DOCBOOK_INSTALL_SUBDIR)"
	$(INSTALL_DATA) html/*.* "$(DESTDIR)$(htmldir)$(DOCBOOK_INSTALL_SUBDIR)"

docbook-uninstall-html:
	rm -f -r "$(DESTDIR)$(htmldir)$(DOCBOOK_INSTALL_SUBDIR)"

endif RELACS_DOCBOOK_HTML_COND


## ------------------------------ ##
## Rules specific for PS output. ##
## ------------------------------ ##

if RELACS_DOCBOOK_PS_COND

DOCBOOK_CLEAN_PS = $(DOCBOOK_BASE).ps

docbook-ps: $(DOCBOOK_BASE).ps

$(DOCBOOK_BASE).ps: $(DOCBOOK_BASE).xml $(DOCBOOK_BASE).xsl $(DOCBOOK_FILES)
	cd $(srcdir); \
	xmlto -m $(abs_builddir)/$(DOCBOOK_BASE).xsl --skip-validation ps $(abs_builddir)/$(DOCBOOK_BASE).xml

docbook-install-ps:
	test -z "$(psdir)" || $(MKDIR_P) "$(DESTDIR)$(psdir)$(DOCBOOK_INSTALL_SUBDIR)"
	$(INSTALL_DATA) "$(DOCBOOK_BASE).ps" "$(DESTDIR)$(psdir)$(DOCBOOK_INSTALL_SUBDIR)/"

docbook-uninstall-ps:
	rm -f "$(DESTDIR)$(psdir)$(DOCBOOK_INSTALL_SUBDIR)/$(DOCBOOK_BASE).ps"

endif RELACS_DOCBOOK_PS_COND


## ------------------------------ ##
## Rules specific for PDF output. ##
## ------------------------------ ##

if RELACS_DOCBOOK_PDF_COND

DOCBOOK_CLEAN_PDF = $(DOCBOOK_BASE).pdf

docbook-pdf: $(DOCBOOK_BASE).pdf

$(DOCBOOK_BASE).pdf: $(DOCBOOK_BASE).xml $(DOCBOOK_BASE).xsl $(DOCBOOK_FILES)
	cd $(srcdir); \
	xmlto -m $(abs_builddir)/$(DOCBOOK_BASE).xsl --skip-validation pdf $(abs_builddir)/$(DOCBOOK_BASE).xml

docbook-install-pdf:
	test -z "$(pdfdir)" || $(MKDIR_P) "$(DESTDIR)$(pdfdir)$(DOCBOOK_INSTALL_SUBDIR)"
	$(INSTALL_DATA) "$(DOCBOOK_BASE).pdf" "$(DESTDIR)$(pdfdir)$(DOCBOOK_INSTALL_SUBDIR)/"

docbook-uninstall-pdf:
	rm -f "$(DESTDIR)$(pdfdir)$(DOCBOOK_INSTALL_SUBDIR)/$(DOCBOOK_BASE).pdf"

endif RELACS_DOCBOOK_PDF_COND


## ------------------------------------------------- ##
## General targets for making, installing, cleaning. ##
## ------------------------------------------------- ##

.PHONY: docbook-run docbook-doc \
        docbook-install-html docbook-uninstall-html \
        docbook-ps docbook-install-ps docbook-uninstall-ps \
        docbook-pdf docbook-install-pdf docbook-uninstall-pdf \
	docbook-install docbook-uninstall docbook-clean

docbook-run: docbook-html docbook-ps docbook-pdf

docbook-doc: docbook-clean docbook-run

docbook-install: docbook-run docbook-install-html docbook-install-ps docbook-install-pdf

docbook-uninstall: docbook-uninstall-html docbook-uninstall-ps docbook-uninstall-pdf

DOCBOOK_CLEANFILES = \
    $(DOCBOOK_CLEAN_PS) \
    $(DOCBOOK_CLEAN_PDF) \
    -r \
    $(DOCBOOK_CLEAN_HTML)

docbook-clean:
	-rm -f $(DOCBOOK_CLEANFILES)

endif RELACS_DOCBOOK_COND
