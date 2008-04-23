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

DX_PROJECT = datafile
DX_CONFIG = doc/$(DX_PROJECT).dox
DX_INSTALL_SUBDIR = /$(DX_PROJECT)

if RELACS_TOP_BUILD
DX_TAGFILES="../numerics/doc/numerics.tag=../numerics ../options/doc/options.tag=../options"
else
DX_TAGFILES="$(DESTDIR)$(docdir)/numerics.tag=../numerics $(DESTDIR)$(docdir)/options.tag=../options"
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
