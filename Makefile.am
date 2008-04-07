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
    m4/relacs_link_tests.m4 \
    m4/relacs_qt3.m4 \
    moc3.mk \
    doxygen.mk \
    \
    doc/relacs.1 \
    \
    doc/relacsall.dox \
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

include doxygen.mk

if DX_COND_doc

doclinks:
	cd $(builddir)/doc/html; \
	LND="$(LN_S)"; test "$$LND" != "cp -p" || LND="cp -r -p"; \
        for dir in $(DOXYGEN_EXTERNAL); do \
	  test -r $$dir || $$LND ../../$$dir/doc/html $$dir; \
	done

doc: doxygen-doc doclinks
all-local: doclinks doxygen-run
clean-local: doxygen-clean
install-data-local: doxygen-install
uninstall-local: doxygen-uninstall

endif DX_COND_doc
