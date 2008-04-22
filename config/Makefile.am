SUBDIRS = \
    src

EXTRA_DIST = \
    m4/ax_prog_doxygen.m4 \
    m4/relacs_qt3.m4 \
    doxygen.mk \
    moc3.mk \
    \
    doc/config.dox


# This is needed for setting the default docdir to $(datarootdir)/doc/relacs.
# PACKAGE_TARNAME seems not to be used otherwise.
PACKAGE_TARNAME=relacs

DX_PACKAGE = config
DX_INSTALL_SUBDIR = /$(DX_PACKAGE)
DX_CFG = doc/$(DX_PACKAGE).dox

if RELACS_TOP_BUILD
DX_TAGFILES="../options/doc/options.tag=../options ../widgets/doc/widgets.tag=../widgets"
else
DX_TAGFILES="$(DESTDIR)$(docdir)/options.tag=../options $(DESTDIR)$(docdir)/widgets.tag=../widgets"
endif

include doxygen.mk

if DX_COND_doc

doc: doxygen-doc
all-local: doxygen-run
clean-local: doxygen-clean
install-data-local: doxygen-install
uninstall-local: doxygen-uninstall

endif DX_COND_doc
