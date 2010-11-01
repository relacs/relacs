SUBDIRS = src
if RELACS_COND_RTAI
SUBDIRS += module
endif
if RELACS_EXAMPLES_COND
SUBDIRS += examples
endif

EXTRA_DIST = \
    doc/comedi.doc \
    module/Makefile \
    module/model.h \
    module/model.c \
    module/rtmodule.c \
    module/dynclampmodule.c \
    module/irisdynclampmodule.c \
    module/moduledef.h \
    module/loadmodules.sh \
    module/killmodules.sh


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
