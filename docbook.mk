if RELACS_DOCBOOK_COND

## ------------------------------- ##
## Rules specific for HTML output. ##
## ------------------------------- ##

if RELACS_DOCBOOK_HTML_COND

DOCBOOK_CLEAN_HTML = $(DOCBOOK_DIR)/html

docbook-html: $(DOCBOOK_DIR)/html/index.html

$(DOCBOOK_DIR)/html/index.html : $(DOCBOOK_DOCDIR)/$(DOCBOOK_BASE).xml $(DOCBOOK_DOCDIR)/$(DOCBOOK_BASE).xls $(DOCBOOK_SOURCES)
	xmlto -m userman.xsl --skip-validation html userman.xml
	test -f userman.fls && for p in $(<userman.fls); do sed -e 's|^.*</pre|</pre|' $p > tmp.html; mv tmp.html $p; done

docbook-install-html:
	test -z "$(DOCBOOK_DOCDIR)/html" || $(MKDIR_P) "$(DESTDIR)$(htmldir)$(DOCBOOK_INSTALL_SUBDIR)"
	$(INSTALL_DATA) $(DOCBOOK_DOCDIR)/html/*.* "$(DESTDIR)$(htmldir)$(DOCBOOK_INSTALL_SUBDIR)"

docbook-uninstall-html:
	rm -f -r "$(DESTDIR)$(htmldir)$(DOCBOOK_INSTALL_SUBDIR)"

endif RELACS_DOCBOOK_HTML_COND


## ------------------------------ ##
## Rules specific for PDF output. ##
## ------------------------------ ##

if RELACS_DOCBOOK_PDF_COND

DOCBOOK_CLEAN_PDF = $(DOCBOOK_DOCDIR)/$(DOCBOOK_PROJECT).pdf

docbook-pdf: $(DOCBOOK_DOCDIR)/$(DOCBOOK_PROJECT).pdf

$(DOCBOOK_DOCDIR)/$(DOCBOOK_PROJECT).pdf: $(DOCBOOK_DOCDIR)/$(DOCBOOK_PROJECT).tag
	cd $(DOCBOOK_DOCDIR)/latex; \
	rm -f *.aux *.toc *.idx *.ind *.ilg *.log *.out; \
	$(DOCBOOK_PDFLATEX) refman.tex; \
	$(DOCBOOK_MAKEINDEX) refman.idx; \
	$(DOCBOOK_PDFLATEX) refman.tex; \
	countdown=5; \
	while $(DOCBOOK_EGREP) 'Rerun (LaTeX|to get cross-references right)' \
		refman.log > /dev/null 2>&1 \
		&& test $$countdown -gt 0; do \
		$(DOCBOOK_PDFLATEX) refman.tex; \
		countdown=`expr $$countdown - 1`; \
	done; \
	mv refman.pdf ../$(DOCBOOK_PROJECT).pdf

docbook-install-pdf:
	test -z "$(pdfdir)" || $(MKDIR_P) "$(DESTDIR)$(pdfdir)"
	$(INSTALL_DATA) "$(DOCBOOK_DOCDIR)/$(DOCBOOK_PROJECT).pdf" "$(DESTDIR)$(pdfdir)/$(DOCBOOK_PROJECT).pdf"

docbook-uninstall-pdf:
	rm -f "$(DESTDIR)$(pdfdir)/$(DOCBOOK_PROJECT).pdf"

endif RELACS_DOCBOOK_PDF_COND


## ------------------------------------------------- ##
## General targets for making, installing, cleaning. ##
## ------------------------------------------------- ##

.PHONY: docbook-run docbook-doc \
        docbook-install-html docbook-uninstall-html \
        docbook-pdf docbook-install-pdf docbook-uninstall-pdf \
	docbook-install docbook-uninstall docbook-clean

docbook-run: docbook-html docbook-pdf

docbook-doc: docbook-clean docbook-run

docbook-install: docbook-run docbook-install-html docbook-install-pdf

docbook-uninstall: docbook-uninstall-html docbook-uninstall-pdf

DOCBOOK_CLEANFILES = \
    $(DOCBOOK_CLEAN_PDF) \
    -r \
    $(DOCBOOK_CLEAN_HTML)

docbook-clean:
	-rm -f $(DOCBOOK_CLEANFILES)

endif RELACS_DOCBOOK_COND
