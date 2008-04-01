man_MANS = doc/relacs.1

SUBDIRS = \
    linktest \
    \
    numerics \
    daq \
    options \
    datafile \
    config \
    widgets \
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
    m4/relacs_doxygen.m4 \
    m4/relacs_gnu_make.m4 \
    m4/relacs_link_tests.m4 \
    m4/relacs_qt3.m4 \
    moc3.mk \
    doxygen.mk \
    \
    doc/relacs.1 \
    \
    $(DX-CONFIG) \
    doc/c++.doc \
    doc/macros.doc \
    doc/naming.doc \
    doc/no.doc \
    doc/program.doc \
    doc/relacs.doc \
    doc/repros.doc \
    utils/scaninf.doc \
    \
    doc/continuousbutton.png \
    doc/fixedbutton.png \
    doc/optdialog.png \
    doc/optwidget.png \
    doc/relacs1.png \
    doc/relacs2.png

include doxygen.mk

if DX_COND_doc

doc: doxygen-doc
all-local: doxygen-run
clean-local: doxygen-clean
install-data-local: doxygen-install
uninstall-local: doxygen-uninstall

endif DX_COND_doc
