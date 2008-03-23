SUBDIRS = \
    base \
    controls \
    detectors \
    filter \
    models

EXTRA_DIST = \
    doc/common.dox



if RELACS_CREATE_DOC
doc_DATA = $(builddir)/doc/html/*

$(builddir)/doc/html/*:
	[ -f "$(builddir)/doc/html/index.html" ] || doxygen "$(builddir)/doc/common.dox"

$(builddir)/help:
	[ -d "$(builddir)/help" ] || { mkdir "$(builddir)/help" && "$(top_srcdir)/utils/createhelp" "$(builddir)/help" ; }

clean_doc:
	rm -f -R "$(builddir)/doc/html"

docall: $(builddir)/doc/html/* ## $(builddir)/help

all: docall

doc: docall

clean-am: clean_doc

.PHONY : doc docall clean_doc
endif

