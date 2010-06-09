ACLOCAL_AMFLAGS = \
    -I m4 \
    -I numerics/m4 \
    -I plugins/linuxdevices/comedi/m4 \
    -I plugins/linuxdevices/misc/m4

man_MANS = doc/relacs.1

SUBDIRS = \
    numerics \
    options \
    daq \
    datafile \
    widgets \
    plot \
    \
    relacs \
    src \
    \
    plugins \
    \
    utils \
    \
    examples \
    \
    doc/userman

EXTRA_DIST = \
    docbook.mk \
    doxygen.mk \
    moc3.mk \
    \
    relacs.cfg \
    relacsplugins.cfg \
    macros.cfg \
    calib0-0.cfg \
    calib0-1.cfg \
    \
    doc/relacs.1 \
    \
    doc/Doxyfile \
    doc/relacsall.doc

DISTCLEANFILES = relacs.log relacs.log.old


DX_INSTALL_SUBDIR = /api
DX_DEPENDS = Makefile.am
DX_SETTINGS = PROJECT_NAME='Relacs'
DX_SETTINGS += INPUT='$(DOXYGEN_INPUT) $(abs_builddir)'
DX_SETTINGS += IMAGE_PATH='$(DOXYGEN_IMAGEPATHES)'
DX_SETTINGS += PREDEFINED='$(DOXYGEN_PREDEF)'

include doxygen.mk

if DX_COND_doc

.PHONY: doclinks

doc/doclinksstamp:
	@echo "cd $(builddir)/doc/html"; \
	cd $(builddir)/doc/html; \
	LND="$(LN_S)"; test "$$LND" != "cp -p" || LND="cp -r -p"; \
        for dir in $(DOXYGEN_EXTERNAL); do \
	  echo "test -r $$dir || $$LND ../../$$dir/doc/html $${dir##*/}"; \
	  test -r $$dir || $$LND ../../$$dir/doc/html $${dir##*/}; \
	done
	touch doc/doclinksstamp

doclinks: doc/doclinksstamp

doc: doxygen-doc doclinks
all-local: doxygen-runall doclinks
html-local: doxygen-run
ps-local: doxygen-ps
pdf-local: doxygen-pdf
clean-local: doxygen-clean
	rm -f doc/doclinksstamp
install-data-local: doxygen-install
uninstall-local: doxygen-uninstall

endif DX_COND_doc

# whenever you change the prefix, it is sufficient jsut to delete the *.la files:
libclean:
	find -name '*.la' | xargs rm
