SUBDIRS = \
    base \
    controls \
    detectors \
    filter \
    models

EXTRA_DIST = \
    doxygen.mk \
    relacshelp.mk \
    doc/common.dox


# This is needed for setting the default docdir to $(datarootdir)/doc/relacs.
# PACKAGE_TARNAME seems not to be used otherwise.
PACKAGE_TARNAME=relacs

PLUGINSET = Common
DX_PACKAGE = common
DX_INSTALL_SUBDIR = /$(DX_PACKAGE)
DX_CFG = doc/$(DX_PACKAGE).dox

DX_TAGFILES="../doc/relacsall.tag=.."

include doxygen.mk

if DX_COND_doc

include relacshelp.mk

.PHONY: doc help

doc: doxygen-doc
help: help-make

all-local: doxygen-run help-run
clean-local: doxygen-clean help-clean
install-data-local: doxygen-install help-install
uninstall-local: doxygen-uninstall help-uninstall

endif DX_COND_doc
