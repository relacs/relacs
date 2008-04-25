man_MANS = doc/relacs.1

SUBDIRS = \
    numerics \
    daq \
    options \
    datafile \
    widgets \
    config \
    mplot \
    \
    relacs \
    src \
    \
    common \
    hardware \
    auditory \
    \
    utils

EXTRA_DIST = \
    m4/relacs_gnu_make.m4 \
    m4/relacs_qt3.m4 \
    moc3.mk \
    doxygen.mk \
    \
    doc/relacs.1 \
    \
    doc/Doxyfile \
    doc/c++.doc \
    doc/macros.doc \
    doc/naming.doc \
    doc/no.doc \
    doc/program.doc \
    doc/relacsall.doc \
    doc/repros.doc \
    utils/scaninf.doc \
    \
    doc/continuousbutton.png \
    doc/fixedbutton.png \
    doc/relacs1.png \
    doc/relacs2.png

DX_DEPENDS = Makefile.am
DX_SETTINGS = PROJECT_NAME='Relacs'
DX_SETTINGS += INPUT='$(DOXYGEN_INPUT) $(abs_builddir)'
DX_SETTINGS += IMAGE_PATHES='$(DOXYGEN_IMAGEPATHES)'
DX_SETTINGS += PREDEFINED='$(DOXYGEN_PREDEF)'

include doxygen.mk

if DX_COND_doc

.PHONY: doclinks

doc/doclinksstamp:
	@echo "cd $(builddir)/doc/html"; \
	cd $(builddir)/doc/html; \
	LND="$(LN_S)"; test "$$LND" != "cp -p" || LND="cp -r -p"; \
        for dir in $(DOXYGEN_EXTERNAL); do \
	  echo "test -r $$dir || $$LND ../../$$dir/doc/html $$dir"; \
	  test -r $$dir || $$LND ../../$$dir/doc/html $$dir; \
	done
	touch doc/doclinksstamp

doclinks: doc/doclinksstamp

doc: doxygen-doc doclinks
all-local: doxygen-runall doclinks
html-local: doxygen-run
ps-local: doxygen-ps
pdf-local: doxygen-pdf
clean-local: doxygen-clean
install-data-local: doxygen-install
uninstall-local: doxygen-uninstall

endif DX_COND_doc
