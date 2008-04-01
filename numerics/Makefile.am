# This is needed for setting the default docdir to $(datarootdir)/doc/relacs.
# PACKAGE_TARNAME seems not to be used otherwise.
PACKAGE_TARNAME=relacs

SUBDIRS = \
    src

EXTRA_DIST = \
    m4/relacs_gsl.m4


include doxygen.mk

if DX_COND_doc

doc: doxygen-doc
all-local: doxygen-run
clean-local: doxygen-clean
install-data-local: doxygen-install
uninstall-local: doxygen-uninstall

endif DX_COND_doc
