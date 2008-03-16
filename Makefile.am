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
    patch \
    \
    utils

# DIST_SUBDIRS = \
#    library

EXTRA_DIST = \
    doc/relacs.1 \
    \
    moc3.mk



if RELACS_CREATE_DOC

doc_DATA = doc/html/*

.PHONY : doc docall
all: docall
doc: docall
docall: doc/html/*
	
doc/html/*:
	-[ ! -f doc/html/index.html ] && doxygen doc/relacs.dox

endif

