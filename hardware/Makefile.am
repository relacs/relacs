SUBDIRS = \
    src

EXTRA_DIST = \
    m4/relacs_comedi.m4 \
    doxygenhardware.mk \
    doc/hardware.dox

include doxygenhardware.mk

if DX_COND_doc

doc: doxygen-doc
all-local: doxygen-run
clean-local: doxygen-clean
install-data-local: doxygen-install
uninstall-local: doxygen-uninstall

endif DX_COND_doc


