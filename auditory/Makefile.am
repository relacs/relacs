SUBDIRS = \
    base \
    speaker \
    sascha

EXTRA_DIST = \
    doxygen.mk \
    relacshelp.mk \
    doc/Doxyfile \
    doc/transductionchain.jpg


pkglibdir = $(libdir)/relacs
pkgincludedir = $(includedir)/relacs
pkgdatadir=$(datadir)/relacs
docdir = $(datarootdir)/doc/relacs

PLUGINSET = Auditory
DX_PROJECT = auditory
DX_INSTALL_SUBDIR = /$(DX_PROJECT)
DX_DEPENDS = Makefile.am
DX_SETTINGS = PROJECT_NAME='"Relacs Plugins for Auditory Electrophysiology"'
DX_SETTINGS += INPUT='include/relacs base sascha speaker'
DX_SETTINGS += PREDEFINED='$(DOXYGEN_PREDEF)'
DX_SETTINGS += TAGFILES='../doc/relacsall.tag=.. ../common/doc/common.tag=../common'

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
