SUBDIRS = \
    src

EXTRA_DIST = \
    m4/ax_prog_doxygen.m4 \
    m4/relacs_qt3.m4 \
    \
    moc3.mk \
    doxygen.mk \
    \
    doc/widgets.dox \
    doc/optdialog.png \
    doc/optwidget.png


pkglibdir = $(libdir)/relacs
pkgincludedir = $(includedir)/relacs
pkgdatadir=$(datadir)/relacs
docdir = $(datarootdir)/doc/relacs

DX_PROJECT = widgets
DX_CONFIG = doc/$(DX_PROJECT).dox
DX_INSTALL_SUBDIR = /$(DX_PROJECT)

if RELACS_TOP_BUILD
DX_TAGFILES="../options/doc/options.tag=../options"
else
DX_TAGFILES="$(DESTDIR)$(docdir)/options.tag=../options"
endif

include doxygen.mk

if DX_COND_doc

doc: doxygen-doc
all-local: doxygen-runall
clean-local: doxygen-clean
install-data-local: doxygen-install
uninstall-local: doxygen-uninstall

endif DX_COND_doc
