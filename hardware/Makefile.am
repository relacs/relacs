SUBDIRS = \
    src

EXTRA_DIST = \
    m4/relacs_comedi.m4 \
    doxygen.mk \
    relacshelp.mk \
    doc/hardware.dox


pkglibdir = $(libdir)/relacs
pkgincludedir = $(includedir)/relacs
pkgdatadir=$(datadir)/relacs
docdir = $(datarootdir)/doc/relacs

PLUGINSET = Hardware
DX_PACKAGE = hardware
DX_INSTALL_SUBDIR = /$(DX_PACKAGE)
DX_CFG = doc/$(DX_PACKAGE).dox

DX_TAGFILES="../doc/relacsall.tag=.."

include doxygen.mk

if DX_COND_doc

include relacshelp.mk

.PHONY: doc help

doc: doxygen-doc
help: help-make

all-local: doxygen-runall help-run
clean-local: doxygen-clean help-clean
install-data-local: doxygen-install help-install
uninstall-local: doxygen-uninstall help-uninstall

endif DX_COND_doc
