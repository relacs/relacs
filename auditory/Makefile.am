SUBDIRS = \
    base \
    speaker \
    sascha

EXTRA_DIST = \
    doxygenauditory.mk \
    relacshelp.mk \
    doc/auditory.dox \
    doc/transductionchain.jpg

include doxygenauditory.mk

if DX_COND_doc

PLUGINSET=Auditory
include relacshelp.mk

.PHONY: doc help

doc: doxygen-doc
help: help-make

all-local: doxygen-run help-run
clean-local: doxygen-clean help-clean
install-data-local: doxygen-install help-install
uninstall-local: doxygen-uninstall help-uninstall

endif DX_COND_doc
