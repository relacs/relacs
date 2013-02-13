#if RELACS_EXAMPLES_COND
#    EXAMPLES = examples
#endif
SUBDIRS = src $(EXAMPLES)

daqflexcfgdir = $(pkgdatadir)/configs/daqflex
dist_daqflexcfg_DATA = \
    relacs.cfg \
    relacsplugins.cfg \
    macros.cfg

EXTRA_DIST = doc/daqflex.doc

pkglibdir = $(libdir)/relacs
pkgincludedir = $(includedir)/relacs
pkgdatadir=$(datadir)/relacs
docdir = $(datarootdir)/doc/relacs

daqflexlibdir = $(libdir)/daqflex
dist_daqflexlib_DATA = \
	firmware/USB_1208HS.rbf \
	firmware/USB_1608G.rbf \
        doc/daqflex.rules

PLUGINSET = daqflex
DX_PROJECT = daqflex
DX_CONFIG = ../../../doc/Doxyfile
DX_INSTALL_SUBDIR = /api/$(DX_PROJECT)
DX_DEPENDS = Makefile.am src/Makefile.am
DX_SETTINGS = PROJECT_NAME='"DAQFlex Plugins"'
DX_SETTINGS += INPUT='include/relacs/daqflex src doc'
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
