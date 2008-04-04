SUBDIRS = \
    src

EXTRA_DIST = \
    doxygenrelacs.mk \
    \
    doc/relacs.dox

include doxygenrelacs.mk

if DX_COND_doc

doc: doxygen-doc
all-local: doxygen-run
clean-local: doxygen-clean
install-data-local: doxygen-install
uninstall-local: doxygen-uninstall

endif DX_COND_doc
