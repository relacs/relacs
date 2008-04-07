## http://autoconf-archive.cryp.to/ax_prog_doxygen.html

## --------------------------------- ##
## Format-independent Doxygen rules. ##
## --------------------------------- ##

if DX_COND_doc

DOC_PACKAGE = mplot
DOC_INSTALL_SUBDIR = /mplot
DOC_CFG = doc/mplot.dox

## ------------------------------- ##
## Rules specific for HTML output. ##
## ------------------------------- ##

if DX_COND_html

DX_CLEAN_HTML = @DX_DOCDIR@/html
DX_INSTALL_HTML = html

endif DX_COND_html

## ------------------------------ ##
## Rules specific for CHM output. ##
## ------------------------------ ##

if DX_COND_chm

DX_CLEAN_CHM = @DX_DOCDIR@/chm
DX_INSTALL_CHM = chm

if DX_COND_chi

DX_CLEAN_CHI = @DX_DOCDIR@/$(DOC_PACKAGE).chi
DX_INSTALL_CHI = $(DOC_PACKAGE).chi

endif DX_COND_chi

endif DX_COND_chm

## ------------------------------ ##
## Rules specific for MAN output. ##
## ------------------------------ ##

if DX_COND_man

DX_CLEAN_MAN = @DX_DOCDIR@/man
DX_INSTALL_MAN = man

endif DX_COND_man

## ------------------------------ ##
## Rules specific for RTF output. ##
## ------------------------------ ##

if DX_COND_rtf

DX_CLEAN_RTF = @DX_DOCDIR@/rtf
DX_INSTALL_RTF = rtf

endif DX_COND_rtf

## ------------------------------ ##
## Rules specific for XML output. ##
## ------------------------------ ##

if DX_COND_xml

DX_CLEAN_XML = @DX_DOCDIR@/xml
DX_INSTALL_XML = xml

endif DX_COND_xml

## ----------------------------- ##
## Rules specific for PS output. ##
## ----------------------------- ##

if DX_COND_ps

DX_CLEAN_PS = @DX_DOCDIR@/$(DOC_PACKAGE).ps
DX_INSTALL_PS = $(DOC_PACKAGE).ps

DX_PS_GOAL = doxygen-ps

doxygen-ps: @DX_DOCDIR@/$(DOC_PACKAGE).ps

@DX_DOCDIR@/$(DOC_PACKAGE).ps: @DX_DOCDIR@/$(DOC_PACKAGE).tag
	cd @DX_DOCDIR@/latex; \
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
	$(DX_DVIPS) -o ../$(DOC_PACKAGE).ps refman.dvi

endif DX_COND_ps

## ------------------------------ ##
## Rules specific for PDF output. ##
## ------------------------------ ##

if DX_COND_pdf

DX_CLEAN_PDF = @DX_DOCDIR@/$(DOC_PACKAGE).pdf
DX_INSTALL_PDF = $(DOC_PACKAGE).pdf

DX_PDF_GOAL = doxygen-pdf

doxygen-pdf: @DX_DOCDIR@/$(DOC_PACKAGE).pdf

@DX_DOCDIR@/$(DOC_PACKAGE).pdf: @DX_DOCDIR@/$(DOC_PACKAGE).tag
	cd @DX_DOCDIR@/latex; \
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
	mv refman.pdf ../$(DOC_PACKAGE).pdf

endif DX_COND_pdf

## ------------------------------------------------- ##
## Rules specific for LaTeX (shared for PS and PDF). ##
## ------------------------------------------------- ##

if DX_COND_latex

DX_CLEAN_LATEX = @DX_DOCDIR@/latex
DX_INSTALL_LATEX = latex

endif DX_COND_latex

## ------------------------------------------------- ##
## General targets for making, installing, cleaning. ##
## ------------------------------------------------- ##

.PHONY: doxygen-run doxygen-runall doxygen-doc $(DX_PS_GOAL) $(DX_PDF_GOAL) \
	doxygen-install doxygen-uninstall doxygen-clean

.INTERMEDIATE: doxygen-run $(DX_PS_GOAL) $(DX_PDF_GOAL)

doxygen-run: @DX_DOCDIR@/$(DOC_PACKAGE).tag

doxygen-runall: doxygen-run $(DX_PS_GOAL) $(DX_PDF_GOAL)

doxygen-doc: doxygen-clean doxygen-runall

if RELACS_TOP_BUILD
DOC_TAGFILES="../numerics/doc/numerics.tag=../numerics ../daq/doc/daq.tag=../daq ../options/doc/options.tag=../options"
else
DOC_TAGFILES="$(DESTDIR)$(docdir)/numerics.tag=../numerics $(DESTDIR)$(docdir)/daq.tag=../daq $(DESTDIR)$(docdir)/options.tag=../options"
endif

@DX_DOCDIR@/$(DOC_PACKAGE).tag: $(DOC_CFG).in $(pkginclude_HEADERS)
	cd $(srcdir); \
	{ cat $(abs_builddir)/$(DOC_CFG); \
	  for DX_ENV_LINE in $(DX_ENV); do echo $$DX_ENV_LINE; done; \
	  echo "GENERATE_TAGFILE=$(abs_builddir)/@DX_DOCDIR@/$(DOC_PACKAGE).tag"; \
	  echo "TAGFILES=$(DOC_TAGFILES)"; \
	  echo "OUTPUT_DIRECTORY=$(abs_builddir)/@DX_DOCDIR@"; } \
	| $(DX_DOXYGEN) -

DX_INSTALL_FILES = \
    $(DOC_PACKAGE).tag \
    $(DX_INSTALL_CHI) \
    $(DX_INSTALL_PS) \
    $(DX_INSTALL_PDF)

DX_INSTALL_DIRS = \
    $(DX_INSTALL_HTML) \
    $(DX_INSTALL_CHM) \
    $(DX_INSTALL_MAN) \
    $(DX_INSTALL_RTF) \
    $(DX_INSTALL_XML) \
    $(DX_INSTALL_LATEX)

doxygen-install: doxygen-run
	test -z "$(docdir)" || $(MKDIR_P) "$(DESTDIR)$(docdir)"
	@list='$(DX_INSTALL_FILES)'; for p in $$list; do \
	  echo " $(INSTALL_DATA) '@DX_DOCDIR@/$$p' '$(DESTDIR)$(docdir)/$$p'"; \
	  $(INSTALL_DATA) "@DX_DOCDIR@/$$p" "$(DESTDIR)$(docdir)/$$p"; \
	done
	@list='$(DX_INSTALL_DIRS)'; for dir in $$list; do \
	  echo " test -z @DX_DOCDIR@/$$dir || $(MKDIR_P) $(DESTDIR)$(docdir)/$${dir}$(DOC_INSTALL_SUBDIR)"; \
	  test -z "@DX_DOCDIR@/$$dir" || $(MKDIR_P) "$(DESTDIR)$(docdir)/$${dir}$(DOC_INSTALL_SUBDIR)"; \
	  echo " $(INSTALL_DATA) '@DX_DOCDIR@/$$dir/*.*' '$(DESTDIR)$(docdir)/$${dir}$(DOC_INSTALL_SUBDIR)'"; \
	  $(INSTALL_DATA) @DX_DOCDIR@/$$dir/*.* "$(DESTDIR)$(docdir)/$${dir}$(DOC_INSTALL_SUBDIR)"; \
	done

doxygen-uninstall:
	@list='$(DX_INSTALL_FILES)'; for p in $$list; do \
	  echo " rm -f '$(DESTDIR)$(docdir)/$$p'"; \
	  rm -f "$(DESTDIR)$(docdir)/$$p"; \
	done
	@list='$(DX_INSTALL_DIRS)'; for dir in $$list; do \
	  echo " rm -f -r '$(DESTDIR)$(docdir)/$${dir}$(DOC_INSTALL_SUBDIR)'"; \
	  rm -f -r "$(DESTDIR)$(docdir)/$${dir}$(DOC_INSTALL_SUBDIR)"; \
	done

DX_CLEANFILES = \
    @DX_DOCDIR@/$(DOC_PACKAGE).tag \
    $(DX_CLEAN_CHI) \
    $(DX_CLEAN_PS) \
    $(DX_CLEAN_PDF) \
    -r \
    $(DX_CLEAN_HTML) \
    $(DX_CLEAN_CHM) \
    $(DX_CLEAN_MAN) \
    $(DX_CLEAN_RTF) \
    $(DX_CLEAN_XML) \
    $(DX_CLEAN_LATEX)

doxygen-clean:
	-rm -f $(DX_CLEANFILES)

endif DX_COND_doc

##
## COPYLEFT
##
##   Copyright (c) 2007 Oren Ben-Kiki <oren@ben-kiki.org>
##   Fixed and modified 2008 by Jan Benda <j.benda@biologie.hu-berlin.de>
##
##   Copying and distribution of this file, with or without
##   modification, are permitted in any medium without royalty provided
##   the copyright notice and this notice are preserved.
