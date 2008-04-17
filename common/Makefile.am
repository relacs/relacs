SUBDIRS = \
    base \
    controls \
    detectors \
    filter \
    models

EXTRA_DIST = \
    doxygencommon.mk \
    doc/common.dox

include doxygencommon.mk

if DX_COND_doc

doc: doxygen-doc
all-local: doxygen-run
clean-local: doxygen-clean
install-data-local: doxygen-install
uninstall-local: doxygen-uninstall

endif DX_COND_doc

