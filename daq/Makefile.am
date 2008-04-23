SUBDIRS = \
    src

EXTRA_DIST = \
    m4/ax_prog_doxygen.m4 \
    m4/relacs_gsl.m4 \
    doxygen.mk \
    \
    doc/daq.dox

pkglibdir = $(libdir)/relacs
pkgincludedir = $(includedir)/relacs
pkgdatadir=$(datadir)/relacs
docdir = $(datarootdir)/doc/relacs

DX_PROJECT = daq
DX_CONFIG = doc/$(DX_PROJECT).dox
DX_INSTALL_SUBDIR = /$(DX_PROJECT)

if RELACS_TOP_BUILD
DX_TAGFILES="../numerics/doc/numerics.tag=../numerics"
else
DX_TAGFILES="$(DESTDIR)$(docdir)/numerics.tag=../numerics"
endif

include doxygen.mk

if DX_COND_doc

doc: doxygen-doc
all-local: doxygen-runall
clean-local: doxygen-clean
install-data-local: doxygen-install
uninstall-local: doxygen-uninstall

endif DX_COND_doc
