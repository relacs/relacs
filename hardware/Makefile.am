SUBDIRS = \
    src

EXTRA_DIST = \
    m4/relacs_comedi.m4 \
    doxygen.mk \
    relacshelp.mk \
    doc/Doxyfile


pkglibdir = $(libdir)/relacs
pkgincludedir = $(includedir)/relacs
pkgdatadir=$(datadir)/relacs
docdir = $(datarootdir)/doc/relacs

PLUGINSET = Hardware
DX_PROJECT = hardware
DX_INSTALL_SUBDIR = /$(DX_PROJECT)
DX_DEPENDS = Makefile.am
DX_SETTINGS = PROJECT_NAME='"Relacs Plugins for Accessing Hardware"'
DX_SETTINGS += INPUT='src include/relacs doc'
DX_SETTINGS += PREDEFINED='$(DOXYGEN_PREDEF)'
DX_SETTINGS += TAGFILES='../doc/relacsall.tag=..'

include doxygen.mk

if DX_COND_doc

include relacshelp.mk

.PHONY: doc help

doc: doxygen-doc
help: help-make

all-local: doxygen-runall help-run
html-local: doxygen-run
ps-local: doxygen-ps
pdf-local: doxygen-pdf
clean-local: doxygen-clean help-clean
install-data-local: doxygen-install help-install
uninstall-local: doxygen-uninstall help-uninstall

endif DX_COND_doc
