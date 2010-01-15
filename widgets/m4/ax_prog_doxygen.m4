##### http://autoconf-archive.cryp.to/ax_prog_doxygen.html
#
# SYNOPSIS
#
#   DX_INIT_DOXYGEN(PROJECT-NAME, DOXYFILE-PATH, [OUTPUT-DIR])
#   DX_DOXYGEN_FEATURE(ON|OFF)
#   DX_DOT_FEATURE(ON|OFF)
#   DX_HTML_FEATURE(ON|OFF)
#   DX_CHM_FEATURE(ON|OFF)
#   DX_CHI_FEATURE(ON|OFF)
#   DX_MAN_FEATURE(ON|OFF)
#   DX_RTF_FEATURE(ON|OFF)
#   DX_XML_FEATURE(ON|OFF)
#   DX_PDF_FEATURE(ON|OFF)
#   DX_PS_FEATURE(ON|OFF)
#
# DESCRIPTION
#
#   The DX_*_FEATURE macros control the default setting for the given
#   Doxygen feature. Supported features are 'DOXYGEN' itself, 'DOT' for
#   generating graphics, 'HTML' for plain HTML, 'CHM' for compressed
#   HTML help (for MS users), 'CHI' for generating a seperate .chi file
#   by the .chm file, and 'MAN', 'RTF', 'XML', 'PDF' and 'PS' for the
#   appropriate output formats. The environment variable
#   DOXYGEN_PAPER_SIZE may be specified to override the default
#   'a4wide' paper size.
#
#   By default, HTML, PDF and PS documentation is generated as this
#   seems to be the most popular and portable combination. MAN pages
#   created by Doxygen are usually problematic, though by picking an
#   appropriate subset and doing some massaging they might be better
#   than nothing. CHM and RTF are specific for MS (note that you can't
#   generate both HTML and CHM at the same time). The XML is rather
#   useless unless you apply specialized post-processing to it.
#
#   The macros mainly control the default state of the feature. The use
#   can override the default by specifying --enable or --disable. The
#   macros ensure that contradictory flags are not given (e.g.,
#   --enable-doxygen-html and --enable-doxygen-chm,
#   --enable-doxygen-anything with --disable-doxygen, etc.) Finally,
#   each feature will be automatically disabled (with a warning) if the
#   required programs are missing.
#
#   Once all the feature defaults have been specified, call
#   DX_INIT_DOXYGEN with the following parameters: a one-word name for
#   the project for use as a filename base etc., an optional
#   configuration file name (the default is 'Doxyfile', the same as
#   Doxygen's default), and an optional output directory name (the
#   default is 'doxygen-doc').
#
#   Automake Support
#
#   The following is a template doxygen.mk file to be included in an 
#   Automake Makefile.am file. Make targets and variables values are 
#   controlled by the various DX_COND_* conditionals set by autoconf.
#
#   The provided targets are:
#
#     doxygen-doc: Force to generate all doxygen documentation 
#                  (doxygen-runall).
#
#     doxygen-run: Run doxygen if the doxygen configuration file,
#                  $(pkginclude_HEADERS), or $(DX_DEPENDS) changed. 
#                  This will generate some of the documentation
#                  (HTML, CHM, CHI, MAN, RTF, XML) but will not do
#                  the post processing required for the rest of it 
#                  (PS, PDF, and some MAN).
#
#     doxygen-runall: Run doxygen if the doxygen configuration file,
#                  $(pkginclude_HEADERS), or $(DX_DEPENDS) changed.
#                  This will generate the complete documentation
#                  (HTML, CHM, CHI, MAN, RTF, XML, PS, PDF, and some MAN).
#
#     doxygen-man: Rename some doxygen generated man pages.
#
#     doxygen-ps:  Generate doxygen PostScript documentation.
#
#     doxygen-pdf: Generate doxygen PDF documentation.
#
#     doxygen-clean: Remove all generated documentation files.
#
#     doxygen-install: Install the documentation to $(docdir)
#
#     doxygen-uninstall: Uninstall the documentation.
#
#   Note that by default these are not integrated into the automake
#   targets. The following lines in your Makefile.am would do:
#
#     # a subdirectory for the installed documentation:
#     DX_INSTALL_SUBDIR =
#
#     # a list of settings for overwriting the settings of the DOXYFILE
#     # and the ones of the DX_INIT_DOXYGEN macros.
#     # for example: DX_SETTINGS = PROJECT_NAME='My Project' INPUT=mysrc
#     DX_SETTINGS = 
#
#     include doxygen.mk
#
#     if DX_COND_doc
#
#     doc: doxygen-doc
#     all-local: doxygen-runall
#     clean-local: doxygen-clean
#     install-data-local: doxygen-install
#     uninstall-local: doxygen-uninstall
#
#     endif DX_COND_doc
#
#   If doxygen is used to generate man pages, you can achieve
#   this integration by setting man3_MANS to the list of man pages
#   generated and then adding the dependency:
#
#     $(man3_MANS): doxygen-doc
#
#   This will cause make to run doxygen and generate all the
#   documentation.
#
#   The following variables are intended for use in Makefile.am:
#
#     DX_CLEANFILES = everything to clean.
#     DX_INSTALL_FILES = all files to be installed
#                        (relative to DX_DOCDIR).
#     DX_INSTALL_DIRS = all directories to be installed
#                       (relative to DX_DOCDIR).
#
#   Then add this variable to MOSTLYCLEANFILES.
#
#     ----- begin doxygen.mk -------------------------------------
#
#     ## --------------------------------- ##
#     ## Format-independent Doxygen rules. ##
#     ## --------------------------------- ##
#     
#     if DX_COND_doc
#     
#     ## ------------------------------- ##
#     ## Rules specific for HTML output. ##
#     ## ------------------------------- ##
#     
#     if DX_COND_html
#     
#     DX_CLEAN_HTML = $(DX_DOCDIR)/html
#     
#     doxygen-install-html:
#     	test -z "$(DX_DOCDIR)/html" || $(MKDIR_P) "$(DESTDIR)$(htmldir)$(DX_INSTALL_SUBDIR)"
#     	$(INSTALL_DATA) $(DX_DOCDIR)/html/*.* "$(DESTDIR)$(htmldir)$(DX_INSTALL_SUBDIR)"
#     
#     doxygen-uninstall-html:
#     	rm -f -r "$(DESTDIR)$(htmldir)$(DX_INSTALL_SUBDIR)"
#     
#     endif DX_COND_html
#     
#     ## ------------------------------ ##
#     ## Rules specific for CHM output. ##
#     ## ------------------------------ ##
#     
#     if DX_COND_chm
#     
#     DX_CLEAN_CHM = $(DX_DOCDIR)/chm
#     DX_INSTALL_CHM = chm
#     
#     if DX_COND_chi
#     
#     DX_CLEAN_CHI = $(DX_DOCDIR)/$(DX_PROJECT).chi
#     DX_INSTALL_CHI = $(DX_PROJECT).chi
#     
#     endif DX_COND_chi
#     
#     endif DX_COND_chm
#     
#     ## ------------------------------ ##
#     ## Rules specific for MAN output. ##
#     ## ------------------------------ ##
#     
#     if DX_COND_man
#     
#     DX_CLEAN_MAN = $(DX_DOCDIR)/man
#     DX_INSTALL_MAN = man
#     
#     endif DX_COND_man
#     
#     ## ------------------------------ ##
#     ## Rules specific for RTF output. ##
#     ## ------------------------------ ##
#     
#     if DX_COND_rtf
#     
#     DX_CLEAN_RTF = $(DX_DOCDIR)/rtf
#     DX_INSTALL_RTF = rtf
#     
#     endif DX_COND_rtf
#     
#     ## ------------------------------ ##
#     ## Rules specific for XML output. ##
#     ## ------------------------------ ##
#     
#     if DX_COND_xml
#     
#     DX_CLEAN_XML = $(DX_DOCDIR)/xml
#     DX_INSTALL_XML = xml
#     
#     endif DX_COND_xml
#     
#     ## ----------------------------- ##
#     ## Rules specific for PS output. ##
#     ## ----------------------------- ##
#     
#     if DX_COND_ps
#     
#     DX_CLEAN_PS = $(DX_DOCDIR)/$(DX_PROJECT).ps
#     
#     doxygen-ps: $(DX_DOCDIR)/$(DX_PROJECT).ps
#     
#     $(DX_DOCDIR)/$(DX_PROJECT).ps: $(DX_DOCDIR)/$(DX_PROJECT).tag
#     	cd $(DX_DOCDIR)/latex; \
#     	rm -f *.aux *.toc *.idx *.ind *.ilg *.log *.out; \
#     	$(DX_LATEX) refman.tex; \
#     	$(MAKEINDEX_PATH) refman.idx; \
#     	$(DX_LATEX) refman.tex; \
#     	countdown=5; \
#     	while $(DX_EGREP) 'Rerun (LaTeX|to get cross-references right)' \
#     		refman.log > /dev/null 2>&1 \
#     		&& test $$countdown -gt 0; do \
#     		$(DX_LATEX) refman.tex; \
#     		countdown=`expr $$countdown - 1`; \
#     	done; \
#     	$(DX_DVIPS) -o ../$(DX_PROJECT).ps refman.dvi
#     
#     doxygen-install-ps:
#     	test -z "$(psdir)" || $(MKDIR_P) "$(DESTDIR)$(psdir)"
#     	$(INSTALL_DATA) "$(DX_DOCDIR)/$(DX_PROJECT).ps" "$(DESTDIR)$(psdir)/$(DX_PROJECT).ps"
#     
#     doxygen-uninstall-ps:
#     	rm -f "$(DESTDIR)$(psdir)/$(DX_PROJECT).ps"
#     
#     endif DX_COND_ps
#     
#     ## ------------------------------ ##
#     ## Rules specific for PDF output. ##
#     ## ------------------------------ ##
#     
#     if DX_COND_pdf
#     
#     DX_CLEAN_PDF = $(DX_DOCDIR)/$(DX_PROJECT).pdf
#     
#     doxygen-pdf: $(DX_DOCDIR)/$(DX_PROJECT).pdf
#     
#     $(DX_DOCDIR)/$(DX_PROJECT).pdf: $(DX_DOCDIR)/$(DX_PROJECT).tag
#     	cd $(DX_DOCDIR)/latex; \
#     	rm -f *.aux *.toc *.idx *.ind *.ilg *.log *.out; \
#     	$(DX_PDFLATEX) refman.tex; \
#     	$(DX_MAKEINDEX) refman.idx; \
#     	$(DX_PDFLATEX) refman.tex; \
#     	countdown=5; \
#     	while $(DX_EGREP) 'Rerun (LaTeX|to get cross-references right)' \
#     		refman.log > /dev/null 2>&1 \
#     		&& test $$countdown -gt 0; do \
#     		$(DX_PDFLATEX) refman.tex; \
#     		countdown=`expr $$countdown - 1`; \
#     	done; \
#     	mv refman.pdf ../$(DX_PROJECT).pdf
#     
#     doxygen-install-pdf:
#     	test -z "$(pdfdir)" || $(MKDIR_P) "$(DESTDIR)$(pdfdir)"
#     	$(INSTALL_DATA) "$(DX_DOCDIR)/$(DX_PROJECT).pdf" "$(DESTDIR)$(pdfdir)/$(DX_PROJECT).pdf"
#     
#     doxygen-uninstall-pdf:
#     	rm -f "$(DESTDIR)$(pdfdir)/$(DX_PROJECT).pdf"
#     
#     endif DX_COND_pdf
#     
#     ## ------------------------------------------------- ##
#     ## Rules specific for LaTeX (shared for PS and PDF). ##
#     ## ------------------------------------------------- ##
#     
#     if DX_COND_latex
#     
#     DX_CLEAN_LATEX = $(DX_DOCDIR)/latex
#     
#     # There is no extra dvi target!
#     #
#     #doxygen-dvi: $(DX_DOCDIR)/$(DX_PROJECT).dvi
#     #
#     #$(DX_DOCDIR)/$(DX_PROJECT).dvi: $(DX_DOCDIR)/$(DX_PROJECT).tag
#     #	cp $(DX_DOCDIR)/latex/refman.dvi $(DX_DOCDIR)/$(DX_PROJECT).dvi
#     #
#     #doxygen-install-dvi:
#     #	test -z "$(dvidir)" || $(MKDIR_P) "$(DESTDIR)$(dvidir)"
#     #	$(INSTALL_DATA) "$(DX_DOCDIR)/$(DX_PROJECT).dvi" "$(DESTDIR)$(dvidir)/$(DX_PROJECT).dvi"
#     #
#     #doxygen-uninstall-dvi:
#     #	rm -f "$(DESTDIR)$(pdfdir)/$(DX_PROJECT).dvi"
#     
#     endif DX_COND_latex
#     
#     ## ------------------------------------------------- ##
#     ## General targets for making, installing, cleaning. ##
#     ## ------------------------------------------------- ##
#     
#     .PHONY: doxygen-run doxygen-runall doxygen-doc \
#             doxygen-install-html doxygen-uninstall-html \
#             doxygen-dvi doxygen-install-dvi doxygen-uninstall-dvi \
#             doxygen-ps doxygen-install-ps doxygen-uninstall-ps \
#             doxygen-pdf doxygen-install-pdf doxygen-uninstall-pdf \
#     	doxygen-install doxygen-uninstall doxygen-clean
#     
#     .INTERMEDIATE: doxygen-run doxygen-ps doxygen-pdf
#     
#     doxygen-run: $(DX_DOCDIR)/$(DX_PROJECT).tag
#     
#     doxygen-runall: doxygen-run doxygen-dvi doxygen-ps doxygen-pdf
#     
#     doxygen-doc: doxygen-clean doxygen-runall
#     
#     $(DX_DOCDIR)/$(DX_PROJECT).tag: $(DX_CONFIG) $(pkginclude_HEADERS) $(DX_DEPENDS)
#     	cd $(srcdir); \
#     	{ cat $(DX_CONFIG); \
#     	  for DX_ENV_LINE in $(DX_ENV); do echo $$DX_ENV_LINE; done; \
#	  echo "GENERATE_TAGFILE=$(abs_builddir)/$(DX_DOCDIR)/$(DX_PROJECT).tag"; \
#	  echo "OUTPUT_DIRECTORY=$(abs_builddir)/$(DX_DOCDIR)"; \
#	  for DX_ENV_LINE in $(DX_SETTINGS); do echo $$DX_ENV_LINE; done; \
#        } \
#     	| tee $(abs_builddir)/tmp.dox | $(DX_DOXYGEN) -; \
#	mv $(abs_builddir)/tmp.dox $(abs_builddir)/$(DX_CONFIG).all
#     
#     DX_INSTALL_FILES = \
#         $(DX_INSTALL_CHI)
#     
#     DX_INSTALL_DIRS = \
#         $(DX_INSTALL_CHM) \
#         $(DX_INSTALL_MAN) \
#         $(DX_INSTALL_RTF) \
#         $(DX_INSTALL_XML)
#     
#     doxygen-install: doxygen-run doxygen-install-html doxygen-install-dvi doxygen-install-ps doxygen-install-pdf
#     	test -z "$(pkgdatadir)" || $(MKDIR_P) "$(DESTDIR)$(pkgdatadir)"
#     	$(INSTALL_DATA) "$(DX_DOCDIR)/$(DX_PROJECT).tag" "$(DESTDIR)$(pkgdatadir)/$(DX_PROJECT).tag"
#     	test -z "$(docdir)" || $(MKDIR_P) "$(DESTDIR)$(docdir)"
#     	@list='$(DX_INSTALL_FILES)'; for p in $$list; do \
#     	  echo " $(INSTALL_DATA) '$(DX_DOCDIR)/$$p' '$(DESTDIR)$(docdir)/$$p'"; \
#     	  $(INSTALL_DATA) "$(DX_DOCDIR)/$$p" "$(DESTDIR)$(docdir)/$$p"; \
#     	done
#     	@list='$(DX_INSTALL_DIRS)'; for dir in $$list; do \
#     	  echo " test -z $(DX_DOCDIR)/$$dir || $(MKDIR_P) $(DESTDIR)$(docdir)/$${dir}$(DX_INSTALL_SUBDIR)"; \
#     	  test -z "$(DX_DOCDIR)/$$dir" || $(MKDIR_P) "$(DESTDIR)$(docdir)/$${dir}$(DX_INSTALL_SUBDIR)"; \
#     	  echo " $(INSTALL_DATA) '$(DX_DOCDIR)/$$dir/*.*' '$(DESTDIR)$(docdir)/$${dir}$(DX_INSTALL_SUBDIR)'"; \
#     	  $(INSTALL_DATA) $(DX_DOCDIR)/$$dir/*.* "$(DESTDIR)$(docdir)/$${dir}$(DX_INSTALL_SUBDIR)"; \
#     	done
#     
#     doxygen-uninstall: doxygen-uninstall-html doxygen-uninstall-dvi doxygen-uninstall-ps doxygen-uninstall-pdf
#     	rm -f "$(DESTDIR)$(pkgdatadir)/$(DX_PROJECT).tag"
#     	@list='$(DX_INSTALL_FILES)'; for p in $$list; do \
#     	  echo " rm -f '$(DESTDIR)$(docdir)/$$p'"; \
#     	  rm -f "$(DESTDIR)$(docdir)/$$p"; \
#     	done
#     	@list='$(DX_INSTALL_DIRS)'; for dir in $$list; do \
#     	  echo " rm -f -r '$(DESTDIR)$(docdir)/$${dir}$(DX_INSTALL_SUBDIR)'"; \
#     	  rm -f -r "$(DESTDIR)$(docdir)/$${dir}$(DX_INSTALL_SUBDIR)"; \
#     	done
#     
#     DX_CLEANFILES = \
#         $(DX_CONFIG).all \
#         $(DX_DOCDIR)/$(DX_PROJECT).tag \
#         $(DX_CLEAN_CHI) \
#         $(DX_CLEAN_PS) \
#         $(DX_CLEAN_PDF) \
#         -r \
#         $(DX_CLEAN_HTML) \
#         $(DX_CLEAN_CHM) \
#         $(DX_CLEAN_MAN) \
#         $(DX_CLEAN_RTF) \
#         $(DX_CLEAN_XML)
#     
#     doxygen-clean:
#     	-rm -f $(DX_CLEANFILES)
#     
#     endif DX_COND_doc
#     
#     ----- end doxygen.mk ---------------------------------------
#
# LAST MODIFICATION
#
#   2008-04-23
#
# COPYLEFT
#
#   Copyright (c) 2007 Oren Ben-Kiki <oren@ben-kiki.org>
#   Fixed and modified 2008 by Jan Benda <benda@bio.lmu.de>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

## ----------##
## Defaults. ##
## ----------##

DX_ENV=""
AC_DEFUN([DX_FEATURE_doc],  ON)
AC_DEFUN([DX_FEATURE_dot],  ON)
AC_DEFUN([DX_FEATURE_man],  OFF)
AC_DEFUN([DX_FEATURE_html], ON)
AC_DEFUN([DX_FEATURE_chm],  OFF)
AC_DEFUN([DX_FEATURE_chi],  OFF)
AC_DEFUN([DX_FEATURE_rtf],  OFF)
AC_DEFUN([DX_FEATURE_xml],  OFF)
AC_DEFUN([DX_FEATURE_pdf],  ON)
AC_DEFUN([DX_FEATURE_ps],   ON)

## --------------- ##
## Private macros. ##
## --------------- ##

# DX_ENV_APPEND(VARIABLE, VALUE)
# ------------------------------
# Append VARIABLE="VALUE" to DX_ENV for invoking doxygen.
AC_DEFUN([DX_ENV_APPEND], [AC_SUBST([DX_ENV], ["$DX_ENV $1='$2'"])])

# DX_DIRNAME_EXPR
# ---------------
# Expand into a shell expression prints the directory part of a path.
AC_DEFUN([DX_DIRNAME_EXPR],
         [[expr ".$1" : '\(\.\)[^/]*$' \| "x$1" : 'x\(.*\)/[^/]*$']])

# DX_IF_FEATURE(FEATURE, IF-ON, IF-OFF)
# -------------------------------------
# Expands according to the M4 (static) status of the feature.
AC_DEFUN([DX_IF_FEATURE], [ifelse(DX_FEATURE_$1, ON, [$2], [$3])])

# DX_REQUIRE_PROG(VARIABLE, PROGRAM)
# ----------------------------------
# Require the specified program to be found for the DX_CURRENT_FEATURE to work.
AC_DEFUN([DX_REQUIRE_PROG], [
AC_PATH_TOOL([$1], [$2])
if test "$$1$[DX_FLAG_]DX_CURRENT_FEATURE" = 1; then
    AC_MSG_WARN([$2 not found - will not DX_CURRENT_DESCRIPTION])
    AC_SUBST([DX_FLAG_]DX_CURRENT_FEATURE, 0)
fi
])

# DX_TEST_FEATURE(FEATURE)
# ------------------------
# Expand to a shell expression testing whether the feature is active.
AC_DEFUN([DX_TEST_FEATURE], [test "$DX_FLAG_$1" = 1])

# DX_CHECK_DEPEND(REQUIRED_FEATURE, REQUIRED_STATE)
# -------------------------------------------------
# Verify that a required features has the right state before trying to turn on
# the DX_CURRENT_FEATURE.
AC_DEFUN([DX_CHECK_DEPEND], [
test "$DX_FLAG_$1" = "$2" \
|| AC_MSG_ERROR([doxygen-DX_CURRENT_FEATURE ifelse([$2], 1,
                            requires, contradicts) doxygen-DX_CURRENT_FEATURE])
])

# DX_CLEAR_DEPEND(FEATURE, REQUIRED_FEATURE, REQUIRED_STATE)
# ----------------------------------------------------------
# Turn off the DX_CURRENT_FEATURE if the required feature is off.
AC_DEFUN([DX_CLEAR_DEPEND], [
test "$DX_FLAG_$1" = "$2" || AC_SUBST([DX_FLAG_]DX_CURRENT_FEATURE, 0)
])

# DX_FEATURE_ARG(FEATURE, DESCRIPTION,
#                CHECK_DEPEND, CLEAR_DEPEND,
#                REQUIRE, DO-IF-ON, DO-IF-OFF)
# --------------------------------------------
# Parse the command-line option controlling a feature. CHECK_DEPEND is called
# if the user explicitly turns the feature on (and invokes DX_CHECK_DEPEND),
# otherwise CLEAR_DEPEND is called to turn off the default state if a required
# feature is disabled (using DX_CLEAR_DEPEND). REQUIRE performs additional
# requirement tests (DX_REQUIRE_PROG). Finally, an automake flag is set and
# DO-IF-ON or DO-IF-OFF are called according to the final state of the feature.
AC_DEFUN([DX_ARG_ABLE], [
    AC_DEFUN([DX_CURRENT_FEATURE], [$1])
    AC_DEFUN([DX_CURRENT_DESCRIPTION], [$2])
    AC_ARG_ENABLE(doxygen-$1,
                  [AS_HELP_STRING(DX_IF_FEATURE([$1], [--disable-doxygen-$1],
                                                      [--enable-doxygen-$1]),
                                  DX_IF_FEATURE([$1], [don't $2], [$2]))],
                  [
case "$enableval" in
#(
y|Y|yes|Yes|YES)
    AC_SUBST([DX_FLAG_$1], 1)
    $3
;; #(
n|N|no|No|NO)
    AC_SUBST([DX_FLAG_$1], 0)
;; #(
*)
    AC_MSG_ERROR([invalid value '$enableval' given to doxygen-$1])
;;
esac
], [
AC_SUBST([DX_FLAG_$1], [DX_IF_FEATURE([$1], 1, 0)])
$4
])
if DX_TEST_FEATURE([$1]); then
    $5
    :
fi
if DX_TEST_FEATURE([$1]); then
    AM_CONDITIONAL(DX_COND_$1, :)
    $6
    :
else
    AM_CONDITIONAL(DX_COND_$1, false)
    $7
    :
fi
])

## -------------- ##
## Public macros. ##
## -------------- ##

# DX_XXX_FEATURE(DEFAULT_STATE)
# -----------------------------
AC_DEFUN([DX_DOXYGEN_FEATURE], [AC_DEFUN([DX_FEATURE_doc],  [$1])])
AC_DEFUN([DX_MAN_FEATURE],     [AC_DEFUN([DX_FEATURE_man],  [$1])])
AC_DEFUN([DX_HTML_FEATURE],    [AC_DEFUN([DX_FEATURE_html], [$1])])
AC_DEFUN([DX_CHM_FEATURE],     [AC_DEFUN([DX_FEATURE_chm],  [$1])])
AC_DEFUN([DX_CHI_FEATURE],     [AC_DEFUN([DX_FEATURE_chi],  [$1])])
AC_DEFUN([DX_RTF_FEATURE],     [AC_DEFUN([DX_FEATURE_rtf],  [$1])])
AC_DEFUN([DX_XML_FEATURE],     [AC_DEFUN([DX_FEATURE_xml],  [$1])])
AC_DEFUN([DX_XML_FEATURE],     [AC_DEFUN([DX_FEATURE_xml],  [$1])])
AC_DEFUN([DX_PDF_FEATURE],     [AC_DEFUN([DX_FEATURE_pdf],  [$1])])
AC_DEFUN([DX_PS_FEATURE],      [AC_DEFUN([DX_FEATURE_ps],   [$1])])

# DX_INIT_DOXYGEN(PROJECT, [CONFIG-FILE], [OUTPUT-DOC-DIR])
# ---------------------------------------------------------
# PROJECT also serves as the base name for the documentation files.
# The default CONFIG-FILE is "Doxyfile" and OUTPUT-DOC-DIR is "doxygen-doc".
AC_DEFUN([DX_INIT_DOXYGEN], [

# Files:
AC_SUBST([DX_PROJECT], [$1])
AC_SUBST([DX_CONFIG], [ifelse([$2], [], Doxyfile, [$2])])
AC_SUBST([DX_DOCDIR], [ifelse([$3], [], doxygen-doc, [$3])])

# Environment variables used inside doxygen.cfg:
DX_ENV_APPEND(PROJECT_NAME, $DX_PROJECT)
DX_ENV_APPEND(PROJECT_NUMBER, $PACKAGE_VERSION)

# Doxygen itself:
DX_ARG_ABLE(doc, [generate any doxygen documentation],
            [],
            [],
            [DX_REQUIRE_PROG([DX_DOXYGEN], doxygen)
             DX_REQUIRE_PROG([DX_PERL], perl)],
            [DX_ENV_APPEND(PERL_PATH, $DX_PERL)])

# Dot for graphics:
DX_ARG_ABLE(dot, [generate graphics for doxygen documentation],
            [DX_CHECK_DEPEND(doc, 1)],
            [DX_CLEAR_DEPEND(doc, 1)],
            [DX_REQUIRE_PROG([DX_DOT], dot)],
            [DX_ENV_APPEND(HAVE_DOT, YES)
             DX_ENV_APPEND(DOT_PATH, [`DX_DIRNAME_EXPR($DX_DOT)`])],
            [DX_ENV_APPEND(HAVE_DOT, NO)])

# Man pages generation:
DX_ARG_ABLE(man, [generate doxygen manual pages],
            [DX_CHECK_DEPEND(doc, 1)],
            [DX_CLEAR_DEPEND(doc, 1)],
            [],
            [DX_ENV_APPEND(GENERATE_MAN, YES)],
            [DX_ENV_APPEND(GENERATE_MAN, NO)])

# RTF file generation:
DX_ARG_ABLE(rtf, [generate doxygen RTF documentation],
            [DX_CHECK_DEPEND(doc, 1)],
            [DX_CLEAR_DEPEND(doc, 1)],
            [],
            [DX_ENV_APPEND(GENERATE_RTF, YES)],
            [DX_ENV_APPEND(GENERATE_RTF, NO)])

# XML file generation:
DX_ARG_ABLE(xml, [generate doxygen XML documentation],
            [DX_CHECK_DEPEND(doc, 1)],
            [DX_CLEAR_DEPEND(doc, 1)],
            [],
            [DX_ENV_APPEND(GENERATE_XML, YES)],
            [DX_ENV_APPEND(GENERATE_XML, NO)])

# (Compressed) HTML help generation:
DX_ARG_ABLE(chm, [generate doxygen compressed HTML help documentation],
            [DX_CHECK_DEPEND(doc, 1)],
            [DX_CLEAR_DEPEND(doc, 1)],
            [DX_REQUIRE_PROG([DX_HHC], hhc)],
            [DX_ENV_APPEND(HHC_PATH, $DX_HHC)
             DX_ENV_APPEND(GENERATE_HTML, YES)
             DX_ENV_APPEND(GENERATE_HTMLHELP, YES)],
            [DX_ENV_APPEND(GENERATE_HTMLHELP, NO)])

# Seperate CHI file generation.
DX_ARG_ABLE(chi, [generate doxygen seperate compressed HTML help index file],
            [DX_CHECK_DEPEND(chm, 1)],
            [DX_CLEAR_DEPEND(chm, 1)],
            [],
            [DX_ENV_APPEND(GENERATE_CHI, YES)],
            [DX_ENV_APPEND(GENERATE_CHI, NO)])

# Plain HTML pages generation:
DX_ARG_ABLE(html, [generate doxygen plain HTML documentation],
            [DX_CHECK_DEPEND(doc, 1) DX_CHECK_DEPEND(chm, 0)],
            [DX_CLEAR_DEPEND(doc, 1) DX_CLEAR_DEPEND(chm, 0)],
            [],
            [DX_ENV_APPEND(GENERATE_HTML, YES)],
            [DX_TEST_FEATURE(chm) || DX_ENV_APPEND(GENERATE_HTML, NO)])

# PostScript file generation:
DX_ARG_ABLE(ps, [generate doxygen PostScript documentation],
            [DX_CHECK_DEPEND(doc, 1)],
            [DX_CLEAR_DEPEND(doc, 1)],
            [DX_REQUIRE_PROG([DX_LATEX], latex)
             DX_REQUIRE_PROG([DX_MAKEINDEX], makeindex)
             DX_REQUIRE_PROG([DX_DVIPS], dvips)
             DX_REQUIRE_PROG([DX_EGREP], egrep)])

# PDF file generation:
DX_ARG_ABLE(pdf, [generate doxygen PDF documentation],
            [DX_CHECK_DEPEND(doc, 1)],
            [DX_CLEAR_DEPEND(doc, 1)],
            [DX_REQUIRE_PROG([DX_PDFLATEX], pdflatex)
             DX_REQUIRE_PROG([DX_MAKEINDEX], makeindex)
             DX_REQUIRE_PROG([DX_EGREP], egrep)])

# LaTeX generation for PS and/or PDF:
if DX_TEST_FEATURE(ps) || DX_TEST_FEATURE(pdf); then
    AM_CONDITIONAL(DX_COND_latex, :)
    DX_ENV_APPEND(GENERATE_LATEX, YES)
else
    AM_CONDITIONAL(DX_COND_latex, false)
    DX_ENV_APPEND(GENERATE_LATEX, NO)
fi

# Paper size for PS and/or PDF:
AC_ARG_VAR(DOXYGEN_PAPER_SIZE,
           [a4wide (default), a4, letter, legal or executive])
case "$DOXYGEN_PAPER_SIZE" in
#(
"")
    AC_SUBST(DOXYGEN_PAPER_SIZE, "")
;; #(
a4wide|a4|letter|legal|executive)
    DX_ENV_APPEND(PAPER_SIZE, $DOXYGEN_PAPER_SIZE)
;; #(
*)
    AC_MSG_ERROR([unknown DOXYGEN_PAPER_SIZE='$DOXYGEN_PAPER_SIZE'])
;;
esac

DX_SUMMARY=no
if test x$DX_FLAG_doc = x1; then
  DX_SUMMARY="yes ( with"
  test x$DX_FLAG_dot = x1 && DX_SUMMARY="$DX_SUMMARY dot"
  test x$DX_FLAG_man = x1 && DX_SUMMARY="$DX_SUMMARY man"
  test x$DX_FLAG_html = x1 && DX_SUMMARY="$DX_SUMMARY html"
  test x$DX_FLAG_chm = x1 && DX_SUMMARY="$DX_SUMMARY chm"
  test x$DX_FLAG_chi = x1 && DX_SUMMARY="$DX_SUMMARY chi"
  test x$DX_FLAG_rtf = x1 && DX_SUMMARY="$DX_SUMMARY rtf"
  test x$DX_FLAG_xml = x1 && DX_SUMMARY="$DX_SUMMARY xml"
  test x$DX_FLAG_pdf = x1 && DX_SUMMARY="$DX_SUMMARY pdf"
  test x$DX_FLAG_ps = x1 && DX_SUMMARY="$DX_SUMMARY ps"
  DX_SUMMARY="$DX_SUMMARY )"
fi

])
