SUBDIRS = \
    src

EXTRA_DIST = \
    doxygen.mk \
    \
    doc/relacs.dox

DX_PROJECT = relacs
DX_CONFIG = doc/$(DX_PROJECT).dox
DX_INSTALL_SUBDIR = /$(DX_PROJECT)

if RELACS_TOP_BUILD
DX_TAGFILES="../config/doc/config.tag=../config ../daq/doc/daq.tag=../daq ../datafile/doc/datafile.tag=../datafile ../mplot/doc/mplot.tag=../mplot ../numerics/doc/numerics.tag=../numerics ../options/doc/options.tag=../options ../widgets/doc/widgets.tag=../widgets"
else
DX_TAGFILES="$(DESTDIR)$(docdir)/config.tag=../config $(DESTDIR)$(docdir)/daq.tag=../daq $(DESTDIR)$(docdir)/datafile.tag=../datafile $(DESTDIR)$(docdir)/mplot.tag=../mplot $(DESTDIR)$(docdir)/numerics.tag=../numerics $(DESTDIR)$(docdir)/options.tag=../options $(DESTDIR)$(docdir)/widgets.tag=../widgets"
endif

include doxygen.mk

if DX_COND_doc

doc: doxygen-doc
all-local: doxygen-runall
html-local: doxygen-run
ps-local: doxygen-ps
pdf-local: doxygen-pdf
clean-local: doxygen-clean
install-data-local: doxygen-install
uninstall-local: doxygen-uninstall

endif DX_COND_doc
