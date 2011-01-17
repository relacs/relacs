SUBDIRS = src

EXTRA_DIST = \
    doc/auditory.doc \
    relacs.cfg \
    relacsplugins.cfg \
    macros.cfg \
    calib0-0.cfg \
    calib0-1.cfg

DISTCLEANFILES = relacslocal relacs.log relacs.log.old

pkglibdir = $(libdir)/relacs
pkgincludedir = $(includedir)/relacs
pkgdatadir=$(datadir)/relacs
docdir = $(datarootdir)/doc/relacs

PLUGINSET = auditory
DX_PROJECT = auditory
DX_CONFIG = ../../doc/Doxyfile
DX_INSTALL_SUBDIR = /api/$(DX_PROJECT)
DX_DEPENDS = Makefile.am src/Makefile.am
DX_SETTINGS = PROJECT_NAME='"Auditory Relacs Plugins"'
DX_SETTINGS += INPUT='include/relacs/auditory src doc'
DX_SETTINGS += PREDEFINED='$(DOXYGEN_PREDEF)'
DX_SETTINGS += TAGFILES='../../doc/relacsall.tag=.. ../acoustic/doc/acoustic.tag=../acoustic ../ephys/doc/ephys.tag=../ephys ../linuxdevices/misc/doc/misc.tag=../misc'

include $(top_srcdir)/doxygen.mk

if DX_COND_doc

include $(srcdir)/../relacshelp.mk

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
