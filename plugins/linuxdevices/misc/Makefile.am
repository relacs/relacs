if RELACS_EXAMPLES_COND
    EXAMPLES = examples
endif
SUBDIRS = src $(EXAMPLES)

misccfgdir = $(pkgdatadir)/configs/misc
dist_misccfg_DATA = \
    relacs.cfg \
    relacsplugins.cfg \
    macros.cfg

EXTRA_DIST = doc/misc.doc

pkglibdir = $(libdir)/relacs
pkgincludedir = $(includedir)/relacs
pkgdatadir=$(datadir)/relacs
docdir = $(datarootdir)/doc/relacs

PLUGINSET = misc
DX_PROJECT = misc
DX_CONFIG = ../../../doc/Doxyfile
DX_INSTALL_SUBDIR = /api/$(DX_PROJECT)
DX_DEPENDS = Makefile.am src/Makefile.am
DX_SETTINGS = PROJECT_NAME='"Misc Relacs Plugins"'
DX_SETTINGS += INPUT='include/relacs/misc src doc'
DX_SETTINGS += PREDEFINED='$(DOXYGEN_PREDEF)'
DX_SETTINGS += TAGFILES='../../../doc/relacsall.tag=.. ../nieseries/doc/nieseries.tag=..'

include $(top_srcdir)/doxygen.mk

if DX_COND_doc

include $(srcdir)/../../relacshelp.mk

.PHONY: doc help

doc: doxygen-doc
help: doc help-make

all-local: doxygen-runall help-run
html-local: doxygen-run
ps-local: doxygen-ps
pdf-local: doxygen-pdf
clean-local: doxygen-clean help-clean
install-data-local: doxygen-install help-install
uninstall-local: doxygen-uninstall help-uninstall

endif DX_COND_doc
