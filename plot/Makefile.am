if RELACS_EXAMPLES_COND
    EXAMPLES = examples
endif
SUBDIRS = src $(EXAMPLES)

EXTRA_DIST = \
    m4/ax_prog_doxygen.m4 \
    m4/relacs_math.m4 \
    m4/relacs_qt4.m4 \
    doxygen.mk \
    moc4.mk \
    doc/Doxyfile


pkglibdir = $(libdir)/relacs
pkgincludedir = $(includedir)/relacs
pkgdatadir=$(datadir)/relacs
docdir = $(datarootdir)/doc/relacs

DX_PROJECT = plot
DX_INSTALL_SUBDIR = /api/$(DX_PROJECT)
DX_DEPENDS = Makefile.am
DX_SETTINGS = PROJECT_NAME='"Relacs Plot Library"'
DX_SETTINGS += INPUT='src include/relacs doc'
DX_SETTINGS += PREDEFINED='$(DOXYGEN_PREDEF)'

if RELACS_TOP_BUILD
DX_SETTINGS += TAGFILES='../numerics/doc/numerics.tag=../numerics ../daq/doc/daq.tag=../daq ../options/doc/options.tag=../options'
else
DX_SETTINGS += TAGFILES='$(DESTDIR)$(docdir)/numerics.tag=../numerics $(DESTDIR)$(docdir)/daq.tag=../daq $(DESTDIR)$(docdir)/options.tag=../options'
endif

include doxygen.mk

if DX_COND_doc

doc: doxygen-doc
all-local: doxygen-runall
html-local: doxygen-run
ps-local: doxygen-ps
pdf-local: doxygen-pdf
clean-local: doxygen-clean
install-data-local: doxygen-install
uninstall-local: doxygen-uninstall

endif DX_COND_doc
