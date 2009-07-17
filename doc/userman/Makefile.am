
DOCBOOK_BASE = userman

DOCBOOK_INSTALL_SUBDIR = /userman

DOCBOOK_FILES = \
    cpp.xml \
    introduction.xml \
    naming.xml \
    repros.xml \
    utilities.xml \
    gui.xml \
    configuration.xml \
    macros.xml \
    program.xml

DOCBOOK_HTML_EXTRA = \
    $(DOCBOOK_BASE).css \
    continuousbutton.png \
    fixedbutton.png

EXTRA_DIST = $(DOCBOOK_BASE).xml $(DOCBOOK_BASE).xsl \
    $(DOCBOOK_FILES) \
    $(DOCBOOK_HTML_EXTRA)

include $(top_srcdir)/docbook.mk

if RELACS_DOCBOOK_COND

doc: docbook-doc
all-local: docbook-run
html-local: docbook-html
ps-local: docbook-ps
pdf-local: docbook-pdf
clean-local: docbook-clean
install-data-local: docbook-install
uninstall-local: docbook-uninstall

endif RELACS_DOCBOOK_COND
