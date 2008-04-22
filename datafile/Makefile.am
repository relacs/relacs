SUBDIRS = \
    src

EXTRA_DIST = \
    m4/ax_prog_doxygen.m4 \
    doxygen.mk \
    \
    doc/datafile.dox


pkglibdir = $(libdir)/relacs
pkgincludedir = $(includedir)/relacs
pkgdatadir=$(datadir)/relacs
docdir = $(datarootdir)/doc/relacs

DX_PACKAGE = datafile
DX_INSTALL_SUBDIR = /$(DX_PACKAGE)
DX_CFG = doc/$(DX_PACKAGE).dox

if RELACS_TOP_BUILD
DX_TAGFILES="../numerics/doc/numerics.tag=../numerics ../options/doc/options.tag=../options"
else
DX_TAGFILES="$(DESTDIR)$(docdir)/numerics.tag=../numerics $(DESTDIR)$(docdir)/options.tag=../options"
endif

include doxygen.mk

if DX_COND_doc

doc: doxygen-doc
all-local: doxygen-runall
clean-local: doxygen-clean
install-data-local: doxygen-install
uninstall-local: doxygen-uninstall

endif DX_COND_doc
