## http://autoconf-archive.cryp.to/ax_prog_doxygen.html

## --------------------------------- ##
## Format-independent Doxygen rules. ##
## --------------------------------- ##

if DX_COND_doc

## ------------------------------- ##
## Rules specific for HTML output. ##
## ------------------------------- ##

if DX_COND_html

DX_CLEAN_HTML = $(DX_DOCDIR)/html

doxygen-install-html:
	test -z "$(DX_DOCDIR)/html" || $(MKDIR_P) "$(DESTDIR)$(htmldir)$(DX_INSTALL_SUBDIR)"
	$(INSTALL_DATA) $(DX_DOCDIR)/html/*.* "$(DESTDIR)$(htmldir)$(DX_INSTALL_SUBDIR)"

doxygen-uninstall-html:
	rm -f -r "$(DESTDIR)$(htmldir)$(DX_INSTALL_SUBDIR)"

endif DX_COND_html

## ------------------------------ ##
## Rules specific for CHM output. ##
## ------------------------------ ##

if DX_COND_chm

DX_CLEAN_CHM = $(DX_DOCDIR)/chm
DX_INSTALL_CHM = chm

if DX_COND_chi

DX_CLEAN_CHI = $(DX_DOCDIR)/$(DX_PROJECT).chi
DX_INSTALL_CHI = $(DX_PROJECT).chi

endif DX_COND_chi

endif DX_COND_chm

## ------------------------------ ##
## Rules specific for MAN output. ##
## ------------------------------ ##

if DX_COND_man

DX_CLEAN_MAN = $(DX_DOCDIR)/man
DX_INSTALL_MAN = man

endif DX_COND_man

## ------------------------------ ##
## Rules specific for RTF output. ##
## ------------------------------ ##

if DX_COND_rtf

DX_CLEAN_RTF = $(DX_DOCDIR)/rtf
DX_INSTALL_RTF = rtf

endif DX_COND_rtf

## ------------------------------ ##
## Rules specific for XML output. ##
## ------------------------------ ##

if DX_COND_xml

DX_CLEAN_XML = $(DX_DOCDIR)/xml
DX_INSTALL_XML = xml

endif DX_COND_xml

## ----------------------------- ##
## Rules specific for PS output. ##
## ----------------------------- ##

if DX_COND_ps

DX_CLEAN_PS = $(DX_DOCDIR)/$(DX_PROJECT).ps

doxygen-ps: $(DX_DOCDIR)/$(DX_PROJECT).ps

$(DX_DOCDIR)/$(DX_PROJECT).ps: $(DX_DOCDIR)/$(DX_PROJECT).tag
	cd $(DX_DOCDIR)/latex; \
	rm -f *.aux *.toc *.idx *.ind *.ilg *.log *.out; \
	$(DX_LATEX) refman.tex; \
	$(MAKEINDEX_PATH) refman.idx; \
	$(DX_LATEX) refman.tex; \
	countdown=5; \
	while $(DX_EGREP) 'Rerun (LaTeX|to get cross-references right)' \
		refman.log > /dev/null 2>&1 \
		&& test $$countdown -gt 0; do \
		$(DX_LATEX) refman.tex; \
		countdown=`expr $$countdown - 1`; \
	done; \
	$(DX_DVIPS) -o ../$(DX_PROJECT).ps refman.dvi

doxygen-install-ps:
	test -z "$(psdir)" || $(MKDIR_P) "$(DESTDIR)$(psdir)"
	$(INSTALL_DATA) "$(DX_DOCDIR)/$(DX_PROJECT).ps" "$(DESTDIR)$(psdir)/$(DX_PROJECT).ps"

doxygen-uninstall-ps:
	rm -f "$(DESTDIR)$(psdir)/$(DX_PROJECT).ps"

endif DX_COND_ps

## ------------------------------ ##
## Rules specific for PDF output. ##
## ------------------------------ ##

if DX_COND_pdf

DX_CLEAN_PDF = $(DX_DOCDIR)/$(DX_PROJECT).pdf

doxygen-pdf: $(DX_DOCDIR)/$(DX_PROJECT).pdf

$(DX_DOCDIR)/$(DX_PROJECT).pdf: $(DX_DOCDIR)/$(DX_PROJECT).tag
	cd $(DX_DOCDIR)/latex; \
	rm -f *.aux *.toc *.idx *.ind *.ilg *.log *.out; \
	$(DX_PDFLATEX) refman.tex; \
	$(DX_MAKEINDEX) refman.idx; \
	$(DX_PDFLATEX) refman.tex; \
	countdown=5; \
	while $(DX_EGREP) 'Rerun (LaTeX|to get cross-references right)' \
		refman.log > /dev/null 2>&1 \
		&& test $$countdown -gt 0; do \
		$(DX_PDFLATEX) refman.tex; \
		countdown=`expr $$countdown - 1`; \
	done; \
	mv refman.pdf ../$(DX_PROJECT).pdf

doxygen-install-pdf:
	test -z "$(pdfdir)" || $(MKDIR_P) "$(DESTDIR)$(pdfdir)"
	$(INSTALL_DATA) "$(DX_DOCDIR)/$(DX_PROJECT).pdf" "$(DESTDIR)$(pdfdir)/$(DX_PROJECT).pdf"

doxygen-uninstall-pdf:
	rm -f "$(DESTDIR)$(pdfdir)/$(DX_PROJECT).pdf"

endif DX_COND_pdf

## ------------------------------------------------- ##
## Rules specific for LaTeX (shared for PS and PDF). ##
## ------------------------------------------------- ##

if DX_COND_latex

DX_CLEAN_LATEX = $(DX_DOCDIR)/latex

# There is no extra dvi target!
#
#doxygen-dvi: $(DX_DOCDIR)/$(DX_PROJECT).dvi
#
#$(DX_DOCDIR)/$(DX_PROJECT).dvi: $(DX_DOCDIR)/$(DX_PROJECT).tag
#	cp $(DX_DOCDIR)/latex/refman.dvi $(DX_DOCDIR)/$(DX_PROJECT).dvi
#
#doxygen-install-dvi:
#	test -z "$(dvidir)" || $(MKDIR_P) "$(DESTDIR)$(dvidir)"
#	$(INSTALL_DATA) "$(DX_DOCDIR)/$(DX_PROJECT).dvi" "$(DESTDIR)$(dvidir)/$(DX_PROJECT).dvi"
#
#doxygen-uninstall-dvi:
#	rm -f "$(DESTDIR)$(pdfdir)/$(DX_PROJECT).dvi"

endif DX_COND_latex

## ------------------------------------------------- ##
## General targets for making, installing, cleaning. ##
## ------------------------------------------------- ##

.PHONY: doxygen-run doxygen-runall doxygen-doc \
        doxygen-install-html doxygen-uninstall-html \
        doxygen-dvi doxygen-install-dvi doxygen-uninstall-dvi \
        doxygen-ps doxygen-install-ps doxygen-uninstall-ps \
        doxygen-pdf doxygen-install-pdf doxygen-uninstall-pdf \
	doxygen-install doxygen-uninstall doxygen-clean

.INTERMEDIATE: doxygen-run doxygen-ps doxygen-pdf

doxygen-run: $(DX_DOCDIR)/$(DX_PROJECT).tag

doxygen-runall: doxygen-run doxygen-dvi doxygen-ps doxygen-pdf

doxygen-doc: doxygen-clean doxygen-runall

$(DX_DOCDIR)/$(DX_PROJECT).tag: $(DX_CONFIG) $(pkginclude_HEADERS) $(DX_DEPENDS)
	cd $(srcdir); \
	{ cat $(DX_CONFIG); \
	  for DX_ENV_LINE in $(DX_ENV); do echo $$DX_ENV_LINE; done; \
	  echo "GENERATE_TAGFILE=$(abs_builddir)/$(DX_DOCDIR)/$(DX_PROJECT).tag"; \
	  echo "OUTPUT_DIRECTORY=$(abs_builddir)/$(DX_DOCDIR)"; \
	  for DX_ENV_LINE in $(DX_SETTINGS); do echo $$DX_ENV_LINE; done; \
        } \
	| tee $(abs_builddir)/tmp.dox | $(DX_DOXYGEN) -; \
	rm -f $(abs_builddir)/$(DX_CONFIG).all; \
	mv $(abs_builddir)/tmp.dox $(abs_builddir)/$(DX_CONFIG).all

DX_INSTALL_FILES = \
    $(DX_INSTALL_CHI)

DX_INSTALL_DIRS = \
    $(DX_INSTALL_CHM) \
    $(DX_INSTALL_MAN) \
    $(DX_INSTALL_RTF) \
    $(DX_INSTALL_XML)

doxygen-install: doxygen-run doxygen-install-html doxygen-install-dvi doxygen-install-ps doxygen-install-pdf
	test -z "$(pkgdatadir)" || $(MKDIR_P) "$(DESTDIR)$(pkgdatadir)"
	test -z "$(pkgdatadir)/doctags" || $(MKDIR_P) "$(DESTDIR)$(pkgdatadir)/doctags"
	$(INSTALL_DATA) "$(DX_DOCDIR)/$(DX_PROJECT).tag" "$(DESTDIR)$(pkgdatadir)/doctags/$(DX_PROJECT).tag"
	test -z "$(docdir)" || $(MKDIR_P) "$(DESTDIR)$(docdir)"
	@list='$(DX_INSTALL_FILES)'; for p in $$list; do \
	  echo " $(INSTALL_DATA) '$(DX_DOCDIR)/$$p' '$(DESTDIR)$(docdir)/$$p'"; \
	  $(INSTALL_DATA) "$(DX_DOCDIR)/$$p" "$(DESTDIR)$(docdir)/$$p"; \
	done
	@list='$(DX_INSTALL_DIRS)'; for dir in $$list; do \
	  echo " test -z $(DX_DOCDIR)/$$dir || $(MKDIR_P) $(DESTDIR)$(docdir)/$${dir}$(DX_INSTALL_SUBDIR)"; \
	  test -z "$(DX_DOCDIR)/$$dir" || $(MKDIR_P) "$(DESTDIR)$(docdir)/$${dir}$(DX_INSTALL_SUBDIR)"; \
	  echo " $(INSTALL_DATA) '$(DX_DOCDIR)/$$dir/*.*' '$(DESTDIR)$(docdir)/$${dir}$(DX_INSTALL_SUBDIR)'"; \
	  $(INSTALL_DATA) $(DX_DOCDIR)/$$dir/*.* "$(DESTDIR)$(docdir)/$${dir}$(DX_INSTALL_SUBDIR)"; \
	done

doxygen-uninstall: doxygen-uninstall-html doxygen-uninstall-dvi doxygen-uninstall-ps doxygen-uninstall-pdf
	rm -f "$(DESTDIR)$(pkgdatadir)/doctags/$(DX_PROJECT).tag"
	@list='$(DX_INSTALL_FILES)'; for p in $$list; do \
	  echo " rm -f '$(DESTDIR)$(docdir)/$$p'"; \
	  rm -f "$(DESTDIR)$(docdir)/$$p"; \
	done
	@list='$(DX_INSTALL_DIRS)'; for dir in $$list; do \
	  echo " rm -f -r '$(DESTDIR)$(docdir)/$${dir}$(DX_INSTALL_SUBDIR)'"; \
	  rm -f -r "$(DESTDIR)$(docdir)/$${dir}$(DX_INSTALL_SUBDIR)"; \
	done

DX_CLEANFILES = \
    $(DX_CONFIG).all \
    $(DX_DOCDIR)/$(DX_PROJECT).tag \
    $(DX_CLEAN_CHI) \
    $(DX_CLEAN_PS) \
    $(DX_CLEAN_PDF) \
    -r \
    $(DX_CLEAN_HTML) \
    $(DX_CLEAN_CHM) \
    $(DX_CLEAN_MAN) \
    $(DX_CLEAN_RTF) \
    $(DX_CLEAN_XML)

doxygen-clean:
	-rm -f $(DX_CLEANFILES)

endif DX_COND_doc

##
## COPYLEFT
##
##   Copyright (c) 2007 Oren Ben-Kiki <oren@ben-kiki.org>
##   Fixed and modified 2008 by Jan Benda <benda@bio.lmu.de>
##
##   Copying and distribution of this file, with or without
##   modification, are permitted in any medium without royalty provided
##   the copyright notice and this notice are preserved.
