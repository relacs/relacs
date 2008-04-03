SUBDIRS = \
    src

EXTRA_DIST = \
    doxygen.mk \
    \
    doc/core.dox

include doxygen.mk

if DX_COND_doc

doc: doxygen-doc
all-local: doxygen-run
clean-local: doxygen-clean
install-data-local: doxygen-install
uninstall-local: doxygen-uninstall

endif DX_COND_doc
