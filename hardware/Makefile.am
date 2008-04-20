SUBDIRS = \
    src

EXTRA_DIST = \
    m4/relacs_comedi.m4 \
    doxygenhardware.mk \
    relacshelp.mk \
    doc/hardware.dox

include doxygenhardware.mk

if DX_COND_doc

PLUGINSET=hardware
include relacshelp.mk

.PHONY: doc help

doc: doxygen-doc
help: help-make

all-local: doxygen-run help-run
clean-local: doxygen-clean help-clean
install-data-local: doxygen-install help-install
uninstall-local: doxygen-uninstall help-uninstall

endif DX_COND_doc
