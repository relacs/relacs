SUBDIRS = \
    src

EXTRA_DIST = \
    m4/ax_prog_doxygen.m4 \
    m4/relacs_gsl.m4 \
    doxygen.mk \
    \
    doc/numerics.dox


pkglibdir = $(libdir)/relacs
pkgincludedir = $(includedir)/relacs
pkgdatadir=$(datadir)/relacs
docdir = $(datarootdir)/doc/relacs

DX_PACKAGE = numerics
DX_INSTALL_SUBDIR = /$(DX_PACKAGE)
DX_CFG = doc/$(DX_PACKAGE).dox
DX_TAGFILES=""

include doxygen.mk

if DX_COND_doc

doc: doxygen-doc
all-local: doxygen-runall
clean-local: doxygen-clean
install-data-local: doxygen-install
uninstall-local: doxygen-uninstall

endif DX_COND_doc
