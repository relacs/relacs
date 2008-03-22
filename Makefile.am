man_MANS = doc/relacs.1

SUBDIRS = \
    config \
    daq \
    datafile \
    mplot \
    numerics \
    options \
    widgets \
    \
    relacs \
    src \
    \
    common \
    hardware \
    auditory \
    \
    utils

# DIST_SUBDIRS = \
#    library

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

.PHONY : doc docall
all: docall
doc: docall
docall: doc/html/*

$(builddir)/doc/html/*:
	[ -f $(builddir)/doc/html/index.html ] || doxygen $(builddir)/doc/relacs.dox

endif



.PHONY : clean_build_config_dump

clean_build_config_dump:
	rm -f build_config_dump{,in}

distclean: clean_build_config_dump

