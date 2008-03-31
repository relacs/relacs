include doxygen.mk

man_MANS = doc/relacs.1

SUBDIRS = \
    linktest \
    \
    options \
    widgets \
    config \
    numerics \
    daq \
    datafile \
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


if DX_COND_html

doc_DATA = @DX_DOCDIR@/html/*

endif DX_COND_html

doc: doxygen-doc
all-local: doxygen-run
clean-local: doxygen-clean
