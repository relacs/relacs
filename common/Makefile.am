SUBDIRS = \
    base \
    controls \
    detectors \
    filter \
    models

EXTRA_DIST = \
    doxygencommon.mk \
    relacshelp.mk \
    doc/common.dox

include doxygencommon.mk

if DX_COND_doc

PLUGINSET=Common
include relacshelp.mk

.PHONY: doc help

doc: doxygen-doc
help: help-make

all-local: doxygen-run help-run
clean-local: doxygen-clean help-clean
install-data-local: doxygen-install help-install
uninstall-local: doxygen-uninstall help-uninstall

endif DX_COND_doc
