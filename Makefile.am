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

DIST_SUBDIRS = \
    m4

EXTRA_DIST = \
    moc3.mk \
    \
    doc/relacs.1 \
    \
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



if RELACS_CREATE_DOC
doc_DATA = $(builddir)/doc/html/*

$(builddir)/doc/html/*:
	[ -f "$(builddir)/doc/html/index.html" ] || doxygen "$(builddir)/doc/relacs.dox"

clean_doc:
	rm -f -R "$(builddir)/doc/html"

docall: doc/html/*

all: docall

doc: docall

clean-am: clean_doc

.PHONY : doc docall clean_doc
endif

