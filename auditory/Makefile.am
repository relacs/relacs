SUBDIRS = \
    base \
    speaker \
    sascha

EXTRA_DIST = \
    doc/auditory.dox \
    doc/transductionchain.jpg



if RELACS_CREATE_DOC
doc_DATA = $(builddir)/doc/html/*

$(builddir)/doc/html/*:
	[ -f "$(builddir)/doc/html/index.html" ] || doxygen "$(builddir)/doc/auditory.dox"

clean_doc:
	rm -f -R "$(builddir)/doc/html"

docall: doc/html/*

all: docall

doc: docall

clean-am: clean_doc

.PHONY : doc docall clean_doc
endif

