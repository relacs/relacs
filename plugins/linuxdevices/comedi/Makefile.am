if RELACS_COND_COMPILE_comedi
    SDC = src
if RELACS_EXAMPLES_COND
    SDE = examples
endif
endif
SUBDIRS = $(SDC) $(SDE)


comedicfgdir = $(pkgdatadir)/configs/comedi
dist_comedicfg_DATA = \
    relacs.cfg \
    relacsplugins.cfg \
    macros.cfg

EXTRA_DIST = \
    doc/comedi.doc

pkglibdir = $(libdir)/relacs
pkgincludedir = $(includedir)/relacs
pkgdatadir=$(datadir)/relacs
docdir = $(datarootdir)/doc/relacs

PLUGINSET = comedi
DX_PROJECT = comedi
DX_CONFIG = ../../../doc/Doxyfile
DX_INSTALL_SUBDIR = /api/$(DX_PROJECT)
DX_DEPENDS = Makefile.am src/Makefile.am
DX_SETTINGS = PROJECT_NAME='"Comedi Relacs Plugins"'
DX_SETTINGS += INPUT='include/relacs/comedi src doc'
DX_SETTINGS += PREDEFINED='$(DOXYGEN_PREDEF)'
DX_SETTINGS += TAGFILES='../../../doc/relacsall.tag=..'

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
