SUBDIRS = \
    base \
    speaker \
    sascha

EXTRA_DIST = \
    doxygenauditory.mk \
    doc/auditory.dox \
    doc/transductionchain.jpg

include doxygenauditory.mk

if DX_COND_doc

doc: doxygen-doc
all-local: doxygen-run
clean-local: doxygen-clean
install-data-local: doxygen-install
uninstall-local: doxygen-uninstall

endif DX_COND_doc

